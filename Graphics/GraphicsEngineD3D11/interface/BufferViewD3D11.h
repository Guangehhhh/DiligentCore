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
/// Definition of the Diligent::IBufferViewD3D11 interface

#include "../../GraphicsEngine/interface/BufferView.h"

namespace Diligent
{

// {6ABA95FC-CD7D-4C03-8CAE-AFC45F9696B7}
static constexpr INTERFACE_ID IID_BufferViewD3D11 =
{ 0x6aba95fc, 0xcd7d, 0x4c03, { 0x8c, 0xae, 0xaf, 0xc4, 0x5f, 0x96, 0x96, 0xb7 } };

/// Interface to the buffer view object implemented in D3D11
class IBufferViewD3D11 : public IBufferView
{
public:

    /// Returns a pointer to the ID3D11View interface of the internal Direct3D11 object.

    /// The method does *NOT* call AddRef() on the returned interface,
    /// so Release() must not be called.
    virtual ID3D11View* GetD3D11View() = 0;
};

}
