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
/// Definition of the Diligent::IBufferVk interface

#include "../../GraphicsEngine/interface/Buffer.h"

namespace Diligent
{

// {12D8EC02-96F4-431E-9695-C5F572CC7587}
static constexpr INTERFACE_ID IID_BufferVk =
{ 0x12d8ec02, 0x96f4, 0x431e,{ 0x96, 0x95, 0xc5, 0xf5, 0x72, 0xcc, 0x75, 0x87 } };


/// Interface to the buffer object implemented in Vulkan
class IBufferVk : public IBuffer
{
public:

    /// Returns a vulkan buffer handle
    virtual VkBuffer GetVkBuffer()const = 0;

    /// Sets vulkan access flags

    /// \param [in] AccessFlags - Vulkan access flags to be set for this buffer
    virtual void SetAccessFlags(VkAccessFlags AccessFlags) = 0;

    /// If the buffer state is known to the engine (i.e. not Diligent::RESOURCE_STATE_UNKNOWN),
    /// returns Vulkan access flags corresponding to the state. If the state is unknown, returns 0.
    virtual VkAccessFlags GetAccessFlags() const = 0;
};

}
