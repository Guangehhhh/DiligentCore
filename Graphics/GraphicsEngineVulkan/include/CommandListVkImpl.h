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
/// Declaration of Diligent::CommandListVkImpl class

#include "vulkan.h"
#include "CommandListBase.h"
#include "RenderDeviceVkImpl.h"

namespace Diligent
{

/// Implementation of the Diligent::ICommandList interface
class CommandListVkImpl final : public CommandListBase<ICommandList, RenderDeviceVkImpl>
{
public:
    using TCommandListBase = CommandListBase<ICommandList, RenderDeviceVkImpl>;

    CommandListVkImpl(IReferenceCounters*  pRefCounters,
                      RenderDeviceVkImpl*  pDevice,
                      IDeviceContext*      pDeferredCtx,
                      VkCommandBuffer      vkCmdBuff) :
        TCommandListBase {pRefCounters, pDevice},
        m_pDeferredCtx   {pDeferredCtx},
        m_vkCmdBuff      {vkCmdBuff   }
    {
    }
    
    ~CommandListVkImpl()
    {
        VERIFY(m_vkCmdBuff == VK_NULL_HANDLE && !m_pDeferredCtx, "Destroying command list that was never executed");
    }

    void Close(VkCommandBuffer&               CmdBuff,
               RefCntAutoPtr<IDeviceContext>& pDeferredCtx)
    {
        CmdBuff       = m_vkCmdBuff;
        m_vkCmdBuff   = VK_NULL_HANDLE;
        pDeferredCtx  = std::move(m_pDeferredCtx);
    }

private:
    RefCntAutoPtr<IDeviceContext> m_pDeferredCtx;
    VkCommandBuffer m_vkCmdBuff;
};

}
