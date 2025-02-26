/*     Copyright 2019 Diligent Graphics LLC
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF ANY PROPRIETARY RIGHTS.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

#pragma once

/// \file
/// Defines dynamic heap utilities

#include <mutex>
#include <deque>
#include <vector>
#include <atomic>
#include "VariableSizeAllocationsManager.h"
#include "RingBuffer.h"

namespace Diligent
{

namespace DynamicHeap
{

    
// Having global ring buffer shared between all contexts is inconvinient because all contexts
// must share the same frame. Having individual ring bufer per context may result in a lot of unused
// memory. As a result, ring buffer is not currently used for dynamic memory management.
// Instead, every dynamic heap allocates pages from the global dynamic memory manager.
class MasterBlockRingBufferBasedManager
{
public:
    using OffsetType  = RingBuffer::OffsetType;
    using MasterBlock = RingBuffer::OffsetType;
    static constexpr const OffsetType InvalidOffset = RingBuffer::InvalidOffset;

    MasterBlockRingBufferBasedManager(IMemoryAllocator& Allocator, 
                                      Uint32            Size) : 
        m_RingBuffer(Size, Allocator)
    {}

    MasterBlockRingBufferBasedManager            (const MasterBlockRingBufferBasedManager&)  = delete;
    MasterBlockRingBufferBasedManager            (      MasterBlockRingBufferBasedManager&&) = delete;
    MasterBlockRingBufferBasedManager& operator= (const MasterBlockRingBufferBasedManager&)  = delete;
    MasterBlockRingBufferBasedManager& operator= (      MasterBlockRingBufferBasedManager&&) = delete;

    void DiscardMasterBlocks(std::vector<MasterBlock>& /*Blocks*/, Uint64 FenceValue)
    {
        std::lock_guard<std::mutex> Lock{m_RingBufferMtx};
        m_RingBuffer.FinishCurrentFrame(FenceValue);
    }

    void ReleaseStaleBlocks(Uint64 LastCompletedFenceValue)
    {
        std::lock_guard<std::mutex> Lock{m_RingBufferMtx};
        m_RingBuffer.ReleaseCompletedFrames(LastCompletedFenceValue);
    }

    OffsetType GetSize()    const { return m_RingBuffer.GetMaxSize();  }
    OffsetType GetUsedSize()const { return m_RingBuffer.GetUsedSize(); }

protected:
    MasterBlock AllocateMasterBlock(OffsetType SizeInBytes, OffsetType Alignment)
    {
        std::lock_guard<std::mutex> Lock{m_RingBufferMtx};
        return m_RingBuffer.Allocate(SizeInBytes, Alignment);
    }

private:
    std::mutex m_RingBufferMtx;
    RingBuffer m_RingBuffer;
};


class MasterBlockListBasedManager
{
public:
    using OffsetType  = VariableSizeAllocationsManager::OffsetType;
    using MasterBlock = VariableSizeAllocationsManager::Allocation;

    MasterBlockListBasedManager(IMemoryAllocator& Allocator, 
                                Uint32            Size) : 
        m_AllocationsMgr(Size, Allocator)
    {
#ifdef DEVELOPMENT
        m_MasterBlockCounter = 0;
#endif
    }

    MasterBlockListBasedManager            (const MasterBlockListBasedManager&)  = delete;
    MasterBlockListBasedManager            (      MasterBlockListBasedManager&&) = delete;
    MasterBlockListBasedManager& operator= (const MasterBlockListBasedManager&)  = delete;
    MasterBlockListBasedManager& operator= (      MasterBlockListBasedManager&&) = delete;

    ~MasterBlockListBasedManager()
    {
        DEV_CHECK_ERR(m_MasterBlockCounter == 0, m_MasterBlockCounter, " master block(s) have not been returned to the manager");
    }

    template<typename RenderDeviceImplType>
    void ReleaseMasterBlocks(std::vector<MasterBlock>& Blocks, RenderDeviceImplType& Device, Uint64 CmdQueueMask)
    {
        struct StaleMasterBlock
        {
            MasterBlock                  Block;
            MasterBlockListBasedManager* Mgr;

            StaleMasterBlock(MasterBlock&& _Block, MasterBlockListBasedManager* _Mgr)noexcept :
                Block (std::move(_Block)),
                Mgr   (_Mgr)
            {
            }

            StaleMasterBlock            (const StaleMasterBlock&)  = delete;
            StaleMasterBlock& operator= (const StaleMasterBlock&)  = delete;
            StaleMasterBlock& operator= (      StaleMasterBlock&&) = delete;

            StaleMasterBlock(StaleMasterBlock&& rhs)noexcept : 
                Block (std::move(rhs.Block)),
                Mgr   (rhs.Mgr)
            {
                rhs.Block = MasterBlock{};
                rhs.Mgr   = nullptr;
            }

            ~StaleMasterBlock()
            {
                if (Mgr != nullptr)
                {
                    std::lock_guard<std::mutex> Lock{Mgr->m_AllocationsMgrMtx};
#ifdef DEVELOPMENT
                    --Mgr->m_MasterBlockCounter;
#endif
                    Mgr->m_AllocationsMgr.Free(std::move(Block));
                }
            }
        };
        for(auto& Block : Blocks)
        {
            DEV_CHECK_ERR(Block.IsValid(), "Attempting to release invalid master block");
            Device.SafeReleaseDeviceObject(StaleMasterBlock{std::move(Block), this}, CmdQueueMask);
        }
    }

    OffsetType GetSize()    const { return m_AllocationsMgr.GetMaxSize(); }
    OffsetType GetUsedSize()const { return m_AllocationsMgr.GetUsedSize();}

#ifdef DEVELOPMENT
    int32_t GetMasterBlockCounter()const{return m_MasterBlockCounter;}
#endif

protected:
    MasterBlock AllocateMasterBlock(OffsetType SizeInBytes, OffsetType Alignment)
    {
        std::lock_guard<std::mutex> Lock{m_AllocationsMgrMtx};
        auto NewBlock = m_AllocationsMgr.Allocate(SizeInBytes, Alignment);
#ifdef DEVELOPMENT
        if (NewBlock.IsValid())
        {
            ++m_MasterBlockCounter;
        }
#endif
        return NewBlock;
    }

private:
    std::mutex                      m_AllocationsMgrMtx;
    VariableSizeAllocationsManager  m_AllocationsMgr;

#ifdef DEVELOPMENT
    std::atomic_int32_t             m_MasterBlockCounter;
#endif
};

} // namespace DynamicHeap

} // namespace Diligent
