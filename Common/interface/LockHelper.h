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

#include "../../Platforms/interface/Atomics.h"
#include "../../Platforms/Basic/interface/DebugUtilities.h"

namespace ThreadingTools
{

class LockFlag
{
public:
    enum {LOCK_FLAG_UNLOCKED = 0, LOCK_FLAG_LOCKED = 1};
    LockFlag(Atomics::Long InitFlag = LOCK_FLAG_UNLOCKED)noexcept
    {
        //m_Flag.store(InitFlag);
        m_Flag = InitFlag;
    }

    operator Atomics::Long()const{return m_Flag;}

private:
    friend class LockHelper;
    Atomics::AtomicLong m_Flag;
};
  
// Spinlock implementation. This kind of lock should be used in scenarios
// where simultaneous access is uncommon but possible.
class LockHelper
{
public:
    
    LockHelper()noexcept :
        m_pLockFlag(nullptr)
    {
    }
    LockHelper(LockFlag& LockFlag)noexcept :
        m_pLockFlag(nullptr)
    {
        Lock(LockFlag);
    }

    LockHelper( LockHelper&& LockHelper )noexcept :
        m_pLockFlag( std::move(LockHelper.m_pLockFlag) )
    {
        LockHelper.m_pLockFlag = nullptr;
    }

    const LockHelper& operator = (LockHelper&& LockHelper)noexcept
    {
        m_pLockFlag = std::move( LockHelper.m_pLockFlag );
        LockHelper.m_pLockFlag = nullptr;
        return *this;
    }

    ~LockHelper()
    {
        Unlock();
    }

    static bool UnsafeTryLock(LockFlag& LockFlag)noexcept
    {
        return Atomics::AtomicCompareExchange( LockFlag.m_Flag, 
                                               static_cast<Atomics::Long>( LockFlag::LOCK_FLAG_LOCKED ), 
                                               static_cast<Atomics::Long>( LockFlag::LOCK_FLAG_UNLOCKED) ) == LockFlag::LOCK_FLAG_UNLOCKED;
    }

    bool TryLock(LockFlag& LockFlag)noexcept
    {
        if( UnsafeTryLock( LockFlag) )
        {
            m_pLockFlag = &LockFlag;
            return true;
        }
        else 
            return false;
    }
    
    static constexpr const int DefaultSpinCountToYield = 256;

    static void UnsafeLock(LockFlag& LockFlag, int SpinCountToYield = DefaultSpinCountToYield)noexcept
    {
        int SpinCount = 0;
        while( !UnsafeTryLock( LockFlag ) )
        {
            ++SpinCount;
            if(SpinCount == SpinCountToYield)
            {
                SpinCount = 0;
                YieldThread();
            }
        }
    }

    void Lock(LockFlag& LockFlag, int SpinCountToYield = DefaultSpinCountToYield)noexcept
    {
        VERIFY( m_pLockFlag == NULL, "Object already locked" );
        // Wait for the flag to become unlocked and lock it
        int SpinCount = 0;
        while( !TryLock( LockFlag ) )
        {
            ++SpinCount;
            if(SpinCount == SpinCountToYield)
            {
                SpinCount = 0;
                YieldThread();
            }
        }
    }

    static void UnsafeUnlock(LockFlag& LockFlag)noexcept
    {
        LockFlag.m_Flag = LockFlag::LOCK_FLAG_UNLOCKED;
    }

    void Unlock()noexcept
    {
        if( m_pLockFlag )
            UnsafeUnlock(*m_pLockFlag);
        m_pLockFlag = NULL;
    }

private:
    static void YieldThread()noexcept;

    LockFlag* m_pLockFlag;
    LockHelper( const LockHelper& LockHelper );
    const LockHelper& operator = ( const LockHelper& LockHelper );
};

}
