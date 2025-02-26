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
/// Definition of the Diligent::IFenceD3D12 interface

#include "../../GraphicsEngine/interface/Fence.h"

namespace Diligent
{

// {053C0D8C-3757-4220-A9CC-4749EC4794AD}
static constexpr INTERFACE_ID IID_FenceD3D12 =
{ 0x53c0d8c, 0x3757, 0x4220, { 0xa9, 0xcc, 0x47, 0x49, 0xec, 0x47, 0x94, 0xad } };


/// Interface to the fence object implemented in D3D12
class IFenceD3D12 : public IFence
{
public:

    /// Returns a pointer to the ID3D12Fence interface of the internal Direct3D12 object.

    /// The method does *NOT* call AddRef() on the returned interface,
    /// so Release() must not be called.
    virtual ID3D12Fence* GetD3D12Fence() = 0;

    /// Waits until the fence reaches the specified value, on the host.
    virtual void WaitForCompletion(Uint64 Value) = 0;
};

}
