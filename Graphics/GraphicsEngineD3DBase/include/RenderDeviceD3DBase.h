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
/// Implementation of the Diligent::RenderDeviceBase template class and related structures

#include "RenderDeviceBase.h"

namespace Diligent
{

/// Base implementation of a D3D render device

template<typename BaseInterface>
class RenderDeviceD3DBase : public RenderDeviceBase<BaseInterface>
{
public:
    RenderDeviceD3DBase(IReferenceCounters*      pRefCounters, 
                        IMemoryAllocator&        RawMemAllocator, 
                        IEngineFactory*          pEngineFactory,
                        Uint32                   NumDeferredContexts,
                        const DeviceObjectSizes& ObjectSizes) : 
        RenderDeviceBase<BaseInterface>(pRefCounters, RawMemAllocator, pEngineFactory, NumDeferredContexts, ObjectSizes)
    {
        // Flag texture formats always supported in D3D11 and D3D12

#define FLAG_FORMAT(Fmt, IsSupported) m_TextureFormatsInfo[Fmt].Supported=IsSupported

        FLAG_FORMAT(TEX_FORMAT_RGBA32_TYPELESS,            true);
        FLAG_FORMAT(TEX_FORMAT_RGBA32_FLOAT,               true);
        FLAG_FORMAT(TEX_FORMAT_RGBA32_UINT,                true);
        FLAG_FORMAT(TEX_FORMAT_RGBA32_SINT,                true);
        FLAG_FORMAT(TEX_FORMAT_RGB32_TYPELESS,             true);
        FLAG_FORMAT(TEX_FORMAT_RGB32_FLOAT,                true);
        FLAG_FORMAT(TEX_FORMAT_RGB32_UINT,                 true);
        FLAG_FORMAT(TEX_FORMAT_RGB32_SINT,                 true);
        FLAG_FORMAT(TEX_FORMAT_RGBA16_TYPELESS,            true);
        FLAG_FORMAT(TEX_FORMAT_RGBA16_FLOAT,               true);
        FLAG_FORMAT(TEX_FORMAT_RGBA16_UNORM,               true);
        FLAG_FORMAT(TEX_FORMAT_RGBA16_UINT,                true);
        FLAG_FORMAT(TEX_FORMAT_RGBA16_SNORM,               true);
        FLAG_FORMAT(TEX_FORMAT_RGBA16_SINT,                true);
        FLAG_FORMAT(TEX_FORMAT_RG32_TYPELESS,              true);
        FLAG_FORMAT(TEX_FORMAT_RG32_FLOAT,                 true);
        FLAG_FORMAT(TEX_FORMAT_RG32_UINT,                  true);
        FLAG_FORMAT(TEX_FORMAT_RG32_SINT,                  true);
        FLAG_FORMAT(TEX_FORMAT_R32G8X24_TYPELESS,          true);
        FLAG_FORMAT(TEX_FORMAT_D32_FLOAT_S8X24_UINT,       true);
        FLAG_FORMAT(TEX_FORMAT_R32_FLOAT_X8X24_TYPELESS,   true);
        FLAG_FORMAT(TEX_FORMAT_X32_TYPELESS_G8X24_UINT,    true);
        FLAG_FORMAT(TEX_FORMAT_RGB10A2_TYPELESS,           true);
        FLAG_FORMAT(TEX_FORMAT_RGB10A2_UNORM,              true);
        FLAG_FORMAT(TEX_FORMAT_RGB10A2_UINT,               true);
        FLAG_FORMAT(TEX_FORMAT_R11G11B10_FLOAT,            true);
        FLAG_FORMAT(TEX_FORMAT_RGBA8_TYPELESS,             true);
        FLAG_FORMAT(TEX_FORMAT_RGBA8_UNORM,                true);
        FLAG_FORMAT(TEX_FORMAT_RGBA8_UNORM_SRGB,           true);
        FLAG_FORMAT(TEX_FORMAT_RGBA8_UINT,                 true);
        FLAG_FORMAT(TEX_FORMAT_RGBA8_SNORM,                true);
        FLAG_FORMAT(TEX_FORMAT_RGBA8_SINT,                 true);
        FLAG_FORMAT(TEX_FORMAT_RG16_TYPELESS,              true);
        FLAG_FORMAT(TEX_FORMAT_RG16_FLOAT,                 true);
        FLAG_FORMAT(TEX_FORMAT_RG16_UNORM,                 true);
        FLAG_FORMAT(TEX_FORMAT_RG16_UINT,                  true);
        FLAG_FORMAT(TEX_FORMAT_RG16_SNORM,                 true);
        FLAG_FORMAT(TEX_FORMAT_RG16_SINT,                  true);
        FLAG_FORMAT(TEX_FORMAT_R32_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_D32_FLOAT,                  true);
        FLAG_FORMAT(TEX_FORMAT_R32_FLOAT,                  true);
        FLAG_FORMAT(TEX_FORMAT_R32_UINT,                   true);
        FLAG_FORMAT(TEX_FORMAT_R32_SINT,                   true);
        FLAG_FORMAT(TEX_FORMAT_R24G8_TYPELESS,             true);
        FLAG_FORMAT(TEX_FORMAT_D24_UNORM_S8_UINT,          true);
        FLAG_FORMAT(TEX_FORMAT_R24_UNORM_X8_TYPELESS,      true);
        FLAG_FORMAT(TEX_FORMAT_X24_TYPELESS_G8_UINT,       true);
        FLAG_FORMAT(TEX_FORMAT_RG8_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_RG8_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_RG8_UINT,                   true);
        FLAG_FORMAT(TEX_FORMAT_RG8_SNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_RG8_SINT,                   true);
        FLAG_FORMAT(TEX_FORMAT_R16_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_R16_FLOAT,                  true);
        FLAG_FORMAT(TEX_FORMAT_D16_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_R16_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_R16_UINT,                   true);
        FLAG_FORMAT(TEX_FORMAT_R16_SNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_R16_SINT,                   true);
        FLAG_FORMAT(TEX_FORMAT_R8_TYPELESS,                true);
        FLAG_FORMAT(TEX_FORMAT_R8_UNORM,                   true);
        FLAG_FORMAT(TEX_FORMAT_R8_UINT,                    true);
        FLAG_FORMAT(TEX_FORMAT_R8_SNORM,                   true);
        FLAG_FORMAT(TEX_FORMAT_R8_SINT,                    true);
        FLAG_FORMAT(TEX_FORMAT_A8_UNORM,                   true);
        FLAG_FORMAT(TEX_FORMAT_R1_UNORM,                   true);
        FLAG_FORMAT(TEX_FORMAT_RGB9E5_SHAREDEXP,           true);
        FLAG_FORMAT(TEX_FORMAT_RG8_B8G8_UNORM,             true);
        FLAG_FORMAT(TEX_FORMAT_G8R8_G8B8_UNORM,            true);
        FLAG_FORMAT(TEX_FORMAT_BC1_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_BC1_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC1_UNORM_SRGB,             true);
        FLAG_FORMAT(TEX_FORMAT_BC2_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_BC2_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC2_UNORM_SRGB,             true);
        FLAG_FORMAT(TEX_FORMAT_BC3_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_BC3_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC3_UNORM_SRGB,             true);
        FLAG_FORMAT(TEX_FORMAT_BC4_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_BC4_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC4_SNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC5_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_BC5_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC5_SNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_B5G6R5_UNORM,               true);
        FLAG_FORMAT(TEX_FORMAT_B5G5R5A1_UNORM,             true);
        FLAG_FORMAT(TEX_FORMAT_BGRA8_UNORM,                true);
        FLAG_FORMAT(TEX_FORMAT_BGRX8_UNORM,                true);
        FLAG_FORMAT(TEX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, true);
        FLAG_FORMAT(TEX_FORMAT_BGRA8_TYPELESS,             true);
        FLAG_FORMAT(TEX_FORMAT_BGRA8_UNORM_SRGB,           true);
        FLAG_FORMAT(TEX_FORMAT_BGRX8_TYPELESS,             true);
        FLAG_FORMAT(TEX_FORMAT_BGRX8_UNORM_SRGB,           true);
        FLAG_FORMAT(TEX_FORMAT_BC6H_TYPELESS,              true);
        FLAG_FORMAT(TEX_FORMAT_BC6H_UF16,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC6H_SF16,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC7_TYPELESS,               true);
        FLAG_FORMAT(TEX_FORMAT_BC7_UNORM,                  true);
        FLAG_FORMAT(TEX_FORMAT_BC7_UNORM_SRGB,             true);
#undef FLAG_FORMAT
    }
};

}
