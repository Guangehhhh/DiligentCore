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

#include <algorithm>

#include "GraphicsAccessories.h"
#include "DebugUtilities.h"
#include "Align.h"

namespace Diligent
{

const Char* GetValueTypeString( VALUE_TYPE Val )
{
    static const Char* ValueTypeStrings[VT_NUM_TYPES] = {};
    static bool bIsInit = false;
    if( !bIsInit )
    {
#define INIT_VALUE_TYPE_STR( ValType ) ValueTypeStrings[ValType] = #ValType
        INIT_VALUE_TYPE_STR( VT_UNDEFINED );
        INIT_VALUE_TYPE_STR( VT_INT8    );
        INIT_VALUE_TYPE_STR( VT_INT16   );
        INIT_VALUE_TYPE_STR( VT_INT32   );
        INIT_VALUE_TYPE_STR( VT_UINT8   );
        INIT_VALUE_TYPE_STR( VT_UINT16  );
        INIT_VALUE_TYPE_STR( VT_UINT32  );
        INIT_VALUE_TYPE_STR( VT_FLOAT16 );
        INIT_VALUE_TYPE_STR( VT_FLOAT32 );
#undef  INIT_VALUE_TYPE_STR
        static_assert(VT_NUM_TYPES == VT_FLOAT32 + 1, "Not all value type strings initialized.");
        bIsInit = true;
    }

    if( Val >= VT_UNDEFINED && Val < VT_NUM_TYPES )
    {
        return ValueTypeStrings[Val];
    }
    else
    {
        UNEXPECTED( "Incorrect value type (", Val, ")" );
        return "unknown value type";
    }
}

class TexFormatToViewFormatConverter
{
public:
    TexFormatToViewFormatConverter()
    {
        static_assert(TEXTURE_VIEW_SHADER_RESOURCE == 1, "TEXTURE_VIEW_SHADER_RESOURCE == 1 expected");
        static_assert(TEXTURE_VIEW_RENDER_TARGET == 2, "TEXTURE_VIEW_RENDER_TARGET == 2 expected");
        static_assert(TEXTURE_VIEW_DEPTH_STENCIL == 3, "TEXTURE_VIEW_DEPTH_STENCIL == 3 expected");
        static_assert(TEXTURE_VIEW_UNORDERED_ACCESS == 4, "TEXTURE_VIEW_UNORDERED_ACCESS == 4 expected");
#define INIT_TEX_VIEW_FORMAT_INFO(TexFmt, SRVFmt, RTVFmt, DSVFmt, UAVFmt)\
        {\
            m_ViewFormats[ TexFmt ][TEXTURE_VIEW_SHADER_RESOURCE-1]  = TEX_FORMAT_##SRVFmt; \
            m_ViewFormats[ TexFmt ][TEXTURE_VIEW_RENDER_TARGET-1]    = TEX_FORMAT_##RTVFmt; \
            m_ViewFormats[ TexFmt ][TEXTURE_VIEW_DEPTH_STENCIL-1]    = TEX_FORMAT_##DSVFmt; \
            m_ViewFormats[ TexFmt ][TEXTURE_VIEW_UNORDERED_ACCESS-1] = TEX_FORMAT_##UAVFmt; \
        }
        
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_UNKNOWN,                 UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA32_TYPELESS,         RGBA32_FLOAT, RGBA32_FLOAT, UNKNOWN, RGBA32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA32_FLOAT,            RGBA32_FLOAT, RGBA32_FLOAT, UNKNOWN, RGBA32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA32_UINT,             RGBA32_UINT,  RGBA32_UINT,  UNKNOWN, RGBA32_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA32_SINT,             RGBA32_SINT,  RGBA32_SINT,  UNKNOWN, RGBA32_SINT);
                                                                       
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB32_TYPELESS,          RGB32_FLOAT, RGB32_FLOAT, UNKNOWN, RGB32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB32_FLOAT,             RGB32_FLOAT, RGB32_FLOAT, UNKNOWN, RGB32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB32_UINT,              RGB32_UINT,  RGB32_UINT,  UNKNOWN, RGB32_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB32_SINT,              RGB32_SINT,  RGB32_SINT,  UNKNOWN, RGB32_SINT);
                                                                      
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA16_TYPELESS,         RGBA16_FLOAT, RGBA16_FLOAT, UNKNOWN, RGBA16_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA16_FLOAT,            RGBA16_FLOAT, RGBA16_FLOAT, UNKNOWN, RGBA16_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA16_UNORM,            RGBA16_UNORM, RGBA16_UNORM, UNKNOWN, RGBA16_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA16_UINT,             RGBA16_UINT,  RGBA16_UINT,  UNKNOWN, RGBA16_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA16_SNORM,            RGBA16_SNORM, RGBA16_SNORM, UNKNOWN, RGBA16_SNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA16_SINT,             RGBA16_SINT,  RGBA16_SINT,  UNKNOWN, RGBA16_SINT);
                                                                       
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG32_TYPELESS,           RG32_FLOAT, RG32_FLOAT, UNKNOWN, RG32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG32_FLOAT,              RG32_FLOAT, RG32_FLOAT, UNKNOWN, RG32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG32_UINT,               RG32_UINT,  RG32_UINT,  UNKNOWN, RG32_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG32_SINT,               RG32_SINT,  RG32_SINT,  UNKNOWN, RG32_SINT);
                                                                       
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R32G8X24_TYPELESS,       R32_FLOAT_X8X24_TYPELESS, UNKNOWN, D32_FLOAT_S8X24_UINT, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_D32_FLOAT_S8X24_UINT,    R32_FLOAT_X8X24_TYPELESS, UNKNOWN, D32_FLOAT_S8X24_UINT, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R32_FLOAT_X8X24_TYPELESS,R32_FLOAT_X8X24_TYPELESS, UNKNOWN, D32_FLOAT_S8X24_UINT, R32_FLOAT_X8X24_TYPELESS);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_X32_TYPELESS_G8X24_UINT, X32_TYPELESS_G8X24_UINT,  UNKNOWN, D32_FLOAT_S8X24_UINT, X32_TYPELESS_G8X24_UINT);
                                                                       
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB10A2_TYPELESS,        RGB10A2_UNORM, RGB10A2_UNORM, UNKNOWN, RGB10A2_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB10A2_UNORM,           RGB10A2_UNORM, RGB10A2_UNORM, UNKNOWN, RGB10A2_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB10A2_UINT,            RGB10A2_UINT,  RGB10A2_UINT,  UNKNOWN, RGB10A2_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R11G11B10_FLOAT,         R11G11B10_FLOAT, R11G11B10_FLOAT, UNKNOWN, R11G11B10_FLOAT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA8_TYPELESS,          RGBA8_UNORM_SRGB, RGBA8_UNORM_SRGB, UNKNOWN, RGBA8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA8_UNORM,             RGBA8_UNORM,      RGBA8_UNORM,      UNKNOWN, RGBA8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA8_UNORM_SRGB,        RGBA8_UNORM_SRGB, RGBA8_UNORM_SRGB, UNKNOWN, RGBA8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA8_UINT,              RGBA8_UINT,       RGBA8_UINT,       UNKNOWN, RGBA8_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA8_SNORM,             RGBA8_SNORM,      RGBA8_SNORM,      UNKNOWN, RGBA8_SNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGBA8_SINT,              RGBA8_SINT,       RGBA8_SINT,       UNKNOWN, RGBA8_SINT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG16_TYPELESS,           RG16_FLOAT, RG16_FLOAT, UNKNOWN, RG16_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG16_FLOAT,              RG16_FLOAT, RG16_FLOAT, UNKNOWN, RG16_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG16_UNORM,              RG16_UNORM, RG16_UNORM, UNKNOWN, RG16_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG16_UINT,               RG16_UINT,  RG16_UINT,  UNKNOWN, RG16_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG16_SNORM,              RG16_SNORM, RG16_SNORM, UNKNOWN, RG16_SNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG16_SINT,               RG16_SINT,  RG16_SINT,  UNKNOWN, RG16_SINT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R32_TYPELESS,            R32_FLOAT, R32_FLOAT, D32_FLOAT, R32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_D32_FLOAT,               R32_FLOAT, R32_FLOAT, D32_FLOAT, R32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R32_FLOAT,               R32_FLOAT, R32_FLOAT, D32_FLOAT, R32_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R32_UINT,                R32_UINT,  R32_UINT,  UNKNOWN,   R32_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R32_SINT,                R32_SINT,  R32_SINT,  UNKNOWN,   R32_SINT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R24G8_TYPELESS,          R24_UNORM_X8_TYPELESS, UNKNOWN, D24_UNORM_S8_UINT, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_D24_UNORM_S8_UINT,       R24_UNORM_X8_TYPELESS, UNKNOWN, D24_UNORM_S8_UINT, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R24_UNORM_X8_TYPELESS,   R24_UNORM_X8_TYPELESS, UNKNOWN, D24_UNORM_S8_UINT, R24_UNORM_X8_TYPELESS);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_X24_TYPELESS_G8_UINT,    X24_TYPELESS_G8_UINT,  UNKNOWN, D24_UNORM_S8_UINT, X24_TYPELESS_G8_UINT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG8_TYPELESS,            RG8_UNORM, RG8_UNORM, UNKNOWN, RG8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG8_UNORM,               RG8_UNORM, RG8_UNORM, UNKNOWN, RG8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG8_UINT,                RG8_UINT,  RG8_UINT,  UNKNOWN, RG8_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG8_SNORM,               RG8_SNORM, RG8_SNORM, UNKNOWN, RG8_SNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG8_SINT,                RG8_SINT,  RG8_SINT,  UNKNOWN, RG8_SINT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R16_TYPELESS,            R16_FLOAT, R16_FLOAT, UNKNOWN,   R16_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R16_FLOAT,               R16_FLOAT, R16_FLOAT, UNKNOWN,   R16_FLOAT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_D16_UNORM,               R16_UNORM, R16_UNORM, D16_UNORM, R16_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R16_UNORM,               R16_UNORM, R16_UNORM, D16_UNORM, R16_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R16_UINT,                R16_UINT,  R16_UINT,  UNKNOWN,   R16_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R16_SNORM,               R16_SNORM, R16_SNORM, UNKNOWN,   R16_SNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R16_SINT,                R16_SINT,  R16_SINT,  UNKNOWN,   R16_SINT);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R8_TYPELESS,             R8_UNORM, R8_UNORM, UNKNOWN, R8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R8_UNORM,                R8_UNORM, R8_UNORM, UNKNOWN, R8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R8_UINT,                 R8_UINT,  R8_UINT,  UNKNOWN, R8_UINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R8_SNORM,                R8_SNORM, R8_SNORM, UNKNOWN, R8_SNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R8_SINT,                 R8_SINT,  R8_SINT,  UNKNOWN, R8_SINT);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_A8_UNORM,                A8_UNORM, A8_UNORM, UNKNOWN, A8_UNORM);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R1_UNORM,                R1_UNORM, R1_UNORM, UNKNOWN, R1_UNORM);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RGB9E5_SHAREDEXP,        RGB9E5_SHAREDEXP, RGB9E5_SHAREDEXP, UNKNOWN, RGB9E5_SHAREDEXP);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_RG8_B8G8_UNORM,          RG8_B8G8_UNORM,  RG8_B8G8_UNORM,  UNKNOWN, RG8_B8G8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_G8R8_G8B8_UNORM,         G8R8_G8B8_UNORM, G8R8_G8B8_UNORM, UNKNOWN, G8R8_G8B8_UNORM);

        // http://www.g-truc.net/post-0335.html
        // http://renderingpipeline.com/2012/07/texture-compression/
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC1_TYPELESS,            BC1_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC1_UNORM,               BC1_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC1_UNORM_SRGB,          BC1_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC2_TYPELESS,            BC2_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC2_UNORM,               BC2_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC2_UNORM_SRGB,          BC2_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC3_TYPELESS,            BC3_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC3_UNORM,               BC3_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC3_UNORM_SRGB,          BC3_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC4_TYPELESS,            BC4_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC4_UNORM,               BC4_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC4_SNORM,               BC4_SNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC5_TYPELESS,            BC5_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC5_UNORM,               BC5_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC5_SNORM,               BC5_SNORM,      UNKNOWN, UNKNOWN, UNKNOWN);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_B5G6R5_UNORM,            B5G6R5_UNORM,   B5G6R5_UNORM,    UNKNOWN, B5G6R5_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_B5G5R5A1_UNORM,          B5G5R5A1_UNORM, B5G5R5A1_UNORM,  UNKNOWN, B5G5R5A1_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BGRA8_UNORM,             BGRA8_UNORM,    BGRA8_UNORM,     UNKNOWN, BGRA8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BGRX8_UNORM,             BGRX8_UNORM,    BGRX8_UNORM,     UNKNOWN, BGRX8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, R10G10B10_XR_BIAS_A2_UNORM, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BGRA8_TYPELESS,          BGRA8_UNORM_SRGB, BGRA8_UNORM_SRGB, UNKNOWN, BGRA8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BGRA8_UNORM_SRGB,        BGRA8_UNORM_SRGB, BGRA8_UNORM_SRGB, UNKNOWN, BGRA8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BGRX8_TYPELESS,          BGRX8_UNORM_SRGB, BGRX8_UNORM_SRGB, UNKNOWN, BGRX8_UNORM);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BGRX8_UNORM_SRGB,        BGRX8_UNORM_SRGB, BGRX8_UNORM_SRGB, UNKNOWN, BGRX8_UNORM);

        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC6H_TYPELESS,           BC6H_UF16,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC6H_UF16,               BC6H_UF16,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC6H_SF16,               BC6H_SF16,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC7_TYPELESS,            BC7_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC7_UNORM,               BC7_UNORM,      UNKNOWN, UNKNOWN, UNKNOWN);
        INIT_TEX_VIEW_FORMAT_INFO( TEX_FORMAT_BC7_UNORM_SRGB,          BC7_UNORM_SRGB, UNKNOWN, UNKNOWN, UNKNOWN);
#undef INIT_TVIEW_FORMAT_INFO
    }

    TEXTURE_FORMAT GetViewFormat(TEXTURE_FORMAT Format, TEXTURE_VIEW_TYPE ViewType, Uint32 BindFlags)
    {
        VERIFY(ViewType > TEXTURE_VIEW_UNDEFINED && ViewType < TEXTURE_VIEW_NUM_VIEWS, "Unexpected texture view type");
        VERIFY(Format >= TEX_FORMAT_UNKNOWN && Format < TEX_FORMAT_NUM_FORMATS, "Unknown texture format");
        switch (Format)
        {
            case TEX_FORMAT_R16_TYPELESS:
            {
                if (BindFlags & BIND_DEPTH_STENCIL)
                {
                    static TEXTURE_FORMAT D16_ViewFmts[] = 
                    {
                        TEX_FORMAT_R16_UNORM, TEX_FORMAT_R16_UNORM, TEX_FORMAT_D16_UNORM, TEX_FORMAT_R16_UNORM
                    };
                    return D16_ViewFmts[ViewType - 1];
                }
            }

            default: /*do nothing*/break;
        }

        return m_ViewFormats[Format][ViewType-1];
    }

private:

    TEXTURE_FORMAT m_ViewFormats[TEX_FORMAT_NUM_FORMATS][TEXTURE_VIEW_NUM_VIEWS-1];
};

TEXTURE_FORMAT GetDefaultTextureViewFormat(TEXTURE_FORMAT TextureFormat, TEXTURE_VIEW_TYPE ViewType, Uint32 BindFlags)
{
    static TexFormatToViewFormatConverter FmtConverter;
    return FmtConverter.GetViewFormat(TextureFormat, ViewType, BindFlags);
}

const TextureFormatAttribs& GetTextureFormatAttribs( TEXTURE_FORMAT Format )
{
    static TextureFormatAttribs FmtAttribs[TEX_FORMAT_NUM_FORMATS];
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct.
    if( !bIsInit )
    {
#define INIT_TEX_FORMAT_INFO(TexFmt, ComponentSize, NumComponents, ComponentType, IsTypeless, BlockWidth, BlockHeight) \
        FmtAttribs[ TexFmt ] = TextureFormatAttribs(#TexFmt, TexFmt, ComponentSize, NumComponents, ComponentType, IsTypeless, BlockWidth, BlockHeight);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_TYPELESS,         4, 4, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_FLOAT,            4, 4, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_UINT,             4, 4, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_SINT,             4, 4, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_TYPELESS,          4, 3, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_FLOAT,             4, 3, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_UINT,              4, 3, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_SINT,              4, 3, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_TYPELESS,         2, 4, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_FLOAT,            2, 4, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_UNORM,            2, 4, COMPONENT_TYPE_UNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_UINT,             2, 4, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_SNORM,            2, 4, COMPONENT_TYPE_SNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_SINT,             2, 4, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_TYPELESS,           4, 2, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_FLOAT,              4, 2, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_UINT,               4, 2, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_SINT,               4, 2, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32G8X24_TYPELESS,       4, 2, COMPONENT_TYPE_DEPTH_STENCIL,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D32_FLOAT_S8X24_UINT,    4, 2, COMPONENT_TYPE_DEPTH_STENCIL, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_FLOAT_X8X24_TYPELESS,4, 2, COMPONENT_TYPE_DEPTH_STENCIL, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_X32_TYPELESS_G8X24_UINT, 4, 2, COMPONENT_TYPE_DEPTH_STENCIL, false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB10A2_TYPELESS,        4, 1, COMPONENT_TYPE_COMPOUND,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB10A2_UNORM,           4, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB10A2_UINT,            4, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R11G11B10_FLOAT,         4, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_TYPELESS,          1, 4, COMPONENT_TYPE_UNDEFINED,   true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_UNORM,             1, 4, COMPONENT_TYPE_UNORM,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_UNORM_SRGB,        1, 4, COMPONENT_TYPE_UNORM_SRGB, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_UINT,              1, 4, COMPONENT_TYPE_UINT,       false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_SNORM,             1, 4, COMPONENT_TYPE_SNORM,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_SINT,              1, 4, COMPONENT_TYPE_SINT,       false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_TYPELESS,           2, 2, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_FLOAT,              2, 2, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_UNORM,              2, 2, COMPONENT_TYPE_UNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_UINT,               2, 2, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_SNORM,              2, 2, COMPONENT_TYPE_SNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_SINT,               2, 2, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_TYPELESS,            4, 1, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D32_FLOAT,               4, 1, COMPONENT_TYPE_DEPTH,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_FLOAT,               4, 1, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_UINT,                4, 1, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_SINT,                4, 1, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R24G8_TYPELESS,          4, 1, COMPONENT_TYPE_DEPTH_STENCIL,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D24_UNORM_S8_UINT,       4, 1, COMPONENT_TYPE_DEPTH_STENCIL, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R24_UNORM_X8_TYPELESS,   4, 1, COMPONENT_TYPE_DEPTH_STENCIL, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_X24_TYPELESS_G8_UINT,    4, 1, COMPONENT_TYPE_DEPTH_STENCIL, false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_TYPELESS,            1, 2, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_UNORM,               1, 2, COMPONENT_TYPE_UNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_UINT,                1, 2, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_SNORM,               1, 2, COMPONENT_TYPE_SNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_SINT,                1, 2, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_TYPELESS,            2, 1, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_FLOAT,               2, 1, COMPONENT_TYPE_FLOAT,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D16_UNORM,               2, 1, COMPONENT_TYPE_DEPTH,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_UNORM,               2, 1, COMPONENT_TYPE_UNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_UINT,                2, 1, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_SNORM,               2, 1, COMPONENT_TYPE_SNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_SINT,                2, 1, COMPONENT_TYPE_SINT,      false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_TYPELESS,             1, 1, COMPONENT_TYPE_UNDEFINED,  true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_UNORM,                1, 1, COMPONENT_TYPE_UNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_UINT,                 1, 1, COMPONENT_TYPE_UINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_SNORM,                1, 1, COMPONENT_TYPE_SNORM,     false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_SINT,                 1, 1, COMPONENT_TYPE_SINT,      false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_A8_UNORM,                1, 1, COMPONENT_TYPE_UNORM,     false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R1_UNORM,                1, 1, COMPONENT_TYPE_UNORM,    false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB9E5_SHAREDEXP,        4, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_B8G8_UNORM,          1, 4, COMPONENT_TYPE_UNORM,    false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_G8R8_G8B8_UNORM,         1, 4, COMPONENT_TYPE_UNORM,    false, 1,1);

        // http://www.g-truc.net/post-0335.html
        // http://renderingpipeline.com/2012/07/texture-compression/
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC1_TYPELESS,            8,  3, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC1_UNORM,               8,  3, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC1_UNORM_SRGB,          8,  3, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC2_TYPELESS,            16, 4, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC2_UNORM,               16, 4, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC2_UNORM_SRGB,          16, 4, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC3_TYPELESS,            16, 4, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC3_UNORM,               16, 4, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC3_UNORM_SRGB,          16, 4, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC4_TYPELESS,            8,  1, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC4_UNORM,               8,  1, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC4_SNORM,               8,  1, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC5_TYPELESS,            16, 2, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC5_UNORM,               16, 2, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC5_SNORM,               16, 2, COMPONENT_TYPE_COMPRESSED, false, 4,4);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_B5G6R5_UNORM,            2, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_B5G5R5A1_UNORM,          2, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRA8_UNORM,             1, 4, COMPONENT_TYPE_UNORM,    false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRX8_UNORM,             1, 4, COMPONENT_TYPE_UNORM,    false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,  4, 1, COMPONENT_TYPE_COMPOUND, false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRA8_TYPELESS,          1, 4, COMPONENT_TYPE_UNDEFINED,     true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRA8_UNORM_SRGB,        1, 4, COMPONENT_TYPE_UNORM_SRGB,   false, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRX8_TYPELESS,          1, 4, COMPONENT_TYPE_UNDEFINED,     true, 1,1);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRX8_UNORM_SRGB,        1, 4, COMPONENT_TYPE_UNORM_SRGB,   false, 1,1);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC6H_TYPELESS,           16, 3, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC6H_UF16,               16, 3, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC6H_SF16,               16, 3, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC7_TYPELESS,            16, 4, COMPONENT_TYPE_COMPRESSED,  true, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC7_UNORM,               16, 4, COMPONENT_TYPE_COMPRESSED, false, 4,4);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC7_UNORM_SRGB,          16, 4, COMPONENT_TYPE_COMPRESSED, false, 4,4);
#undef  INIT_TEX_FORMAT_INFO
        static_assert(TEX_FORMAT_NUM_FORMATS == TEX_FORMAT_BC7_UNORM_SRGB + 1, "Not all texture formats initialized.");

#ifdef _DEBUG
        for( Uint32 Fmt = TEX_FORMAT_UNKNOWN; Fmt < TEX_FORMAT_NUM_FORMATS; ++Fmt )
            VERIFY(FmtAttribs[Fmt].Format == static_cast<TEXTURE_FORMAT>(Fmt), "Uninitialized format");
#endif

        bIsInit = true;
    }

    if( Format >= TEX_FORMAT_UNKNOWN && Format < TEX_FORMAT_NUM_FORMATS )
    {
        const auto &Attribs = FmtAttribs[Format];
        VERIFY( Attribs.Format == Format, "Unexpected format" );
        return Attribs;
    }
    else
    {
        UNEXPECTED( "Texture format (", int{Format}, ") is out of allowed range [0, ", int{TEX_FORMAT_NUM_FORMATS}-1, "]" );
        return FmtAttribs[0];
    }
}


const Char* GetTexViewTypeLiteralName( TEXTURE_VIEW_TYPE ViewType )
{
    static const Char* TexViewLiteralNames[TEXTURE_VIEW_NUM_VIEWS] = {};
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct.
    if( !bIsInit )
    {
#define INIT_TEX_VIEW_TYPE_NAME(ViewType)TexViewLiteralNames[ViewType] = #ViewType
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_UNDEFINED );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_SHADER_RESOURCE );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_RENDER_TARGET );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_DEPTH_STENCIL );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_UNORDERED_ACCESS );
#undef  INIT_TEX_VIEW_TYPE_NAME
        static_assert(TEXTURE_VIEW_NUM_VIEWS == TEXTURE_VIEW_UNORDERED_ACCESS + 1, "Not all texture views names initialized.");

        bIsInit = true;
    }

    if( ViewType >= TEXTURE_VIEW_UNDEFINED && ViewType < TEXTURE_VIEW_NUM_VIEWS )
    {
        return TexViewLiteralNames[ViewType];
    }
    else
    {
        UNEXPECTED("Texture view type (", ViewType, ") is out of allowed range [0, ", TEXTURE_VIEW_NUM_VIEWS-1, "]" );
        return "<Unknown texture view type>";
    }
}

const Char* GetBufferViewTypeLiteralName( BUFFER_VIEW_TYPE ViewType )
{
    static const Char* BuffViewLiteralNames[BUFFER_VIEW_NUM_VIEWS] = {};
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct.
    if( !bIsInit )
    {
#define INIT_BUFF_VIEW_TYPE_NAME(ViewType)BuffViewLiteralNames[ViewType] = #ViewType
        INIT_BUFF_VIEW_TYPE_NAME( BUFFER_VIEW_UNDEFINED );
        INIT_BUFF_VIEW_TYPE_NAME( BUFFER_VIEW_SHADER_RESOURCE );
        INIT_BUFF_VIEW_TYPE_NAME( BUFFER_VIEW_UNORDERED_ACCESS );
 #undef INIT_BUFF_VIEW_TYPE_NAME
        static_assert(BUFFER_VIEW_NUM_VIEWS == BUFFER_VIEW_UNORDERED_ACCESS + 1, "Not all buffer views names initialized.");

        bIsInit = true;
    }

    if( ViewType >= BUFFER_VIEW_UNDEFINED && ViewType < BUFFER_VIEW_NUM_VIEWS )
    {
        return BuffViewLiteralNames[ViewType];
    }
    else
    {
        UNEXPECTED( "Buffer view type (", ViewType, ") is out of allowed range [0, ", BUFFER_VIEW_NUM_VIEWS-1, "]" );
        return "<Unknown buffer view type>";
    }
}

const Char* GetShaderTypeLiteralName( SHADER_TYPE ShaderType )
{
    switch( ShaderType )
    {
#define RETURN_SHADER_TYPE_NAME(ShaderType)\
        case ShaderType: return #ShaderType;

        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_UNKNOWN )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_VERTEX  )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_PIXEL   )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_GEOMETRY)
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_HULL    )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_DOMAIN  )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_COMPUTE )
#undef  RETURN_SHADER_TYPE_NAME

        default: UNEXPECTED( "Unknown shader type constant ", Uint32{ShaderType} ); return "<Unknown shader type>";
    }
}

String GetShaderStagesString(SHADER_TYPE ShaderStages)
{
    String StagesStr;
    while(ShaderStages != 0)
    for( Uint32 Stage = SHADER_TYPE_VERTEX; ShaderStages != 0 && Stage <= SHADER_TYPE_COMPUTE; Stage <<= 1 )
    {
        if( ShaderStages&Stage )
        {
            if( StagesStr.length() )
                StagesStr += ", ";
            StagesStr += GetShaderTypeLiteralName( static_cast<SHADER_TYPE>(Stage));
            ShaderStages &= ~static_cast<SHADER_TYPE>(Stage);
        }
    }
    VERIFY_EXPR( ShaderStages == 0);
    return StagesStr;
}

const Char* GetShaderVariableTypeLiteralName(SHADER_RESOURCE_VARIABLE_TYPE VarType, bool bGetFullName)
{
    static const Char* ShortVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_NUM_TYPES];
    static const Char* FullVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_NUM_TYPES];
    static bool bVarTypeStrsInit = false;
    if( !bVarTypeStrsInit )
    {
        ShortVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_STATIC]  = "static";
        ShortVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE] = "mutable";
        ShortVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC] = "dynamic";
        FullVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_STATIC]  = "SHADER_RESOURCE_VARIABLE_TYPE_STATIC";
        FullVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE] = "SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE";
        FullVarTypeNameStrings[SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC] = "SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC";

        static_assert(SHADER_RESOURCE_VARIABLE_TYPE_NUM_TYPES == SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC + 1, "Not all shader variable types initialized.");

        bVarTypeStrsInit = true;
    }
    if( VarType >= SHADER_RESOURCE_VARIABLE_TYPE_STATIC && VarType < SHADER_RESOURCE_VARIABLE_TYPE_NUM_TYPES )
        return (bGetFullName ? FullVarTypeNameStrings : ShortVarTypeNameStrings)[VarType];
    else
    {
        UNEXPECTED("Unknow shader variable type");
        return "unknow";
    }
}


const Char* GetShaderResourceTypeLiteralName(SHADER_RESOURCE_TYPE ResourceType, bool bGetFullName)
{
    switch(ResourceType)
    {
        case SHADER_RESOURCE_TYPE_UNKNOWN:         return bGetFullName ?  "SHADER_RESOURCE_TYPE_UNKNOWN"         : "unknown";
        case SHADER_RESOURCE_TYPE_CONSTANT_BUFFER: return bGetFullName ?  "SHADER_RESOURCE_TYPE_CONSTANT_BUFFER" : "constant buffer";
        case SHADER_RESOURCE_TYPE_TEXTURE_SRV:     return bGetFullName ?  "SHADER_RESOURCE_TYPE_TEXTURE_SRV"     : "texture SRV";
        case SHADER_RESOURCE_TYPE_BUFFER_SRV:      return bGetFullName ?  "SHADER_RESOURCE_TYPE_BUFFER_SRV"      : "buffer SRV";
        case SHADER_RESOURCE_TYPE_TEXTURE_UAV:     return bGetFullName ?  "SHADER_RESOURCE_TYPE_TEXTURE_UAV"     : "texture UAV";
        case SHADER_RESOURCE_TYPE_BUFFER_UAV:      return bGetFullName ?  "SHADER_RESOURCE_TYPE_BUFFER_UAV"      : "buffer UAV";
        case SHADER_RESOURCE_TYPE_SAMPLER:         return bGetFullName ?  "SHADER_RESOURCE_TYPE_SAMPLER"         : "sampler";
        default:
            UNEXPECTED("Unexepcted resource type (", Uint32{ResourceType}, ")");
            return "UNKNOWN";
    }
}

const Char* GetMapTypeString(MAP_TYPE MapType)
{
    switch(MapType)
    {
        case MAP_READ:       return "MAP_READ";
        case MAP_WRITE:      return "MAP_WRITE";
        case MAP_READ_WRITE: return "MAP_READ_WRITE";
        
        default:
            UNEXPECTED("Unexpected map type");
            return "Unknown map type";
    }
}

/// Returns the string containing the usage
const Char* GetUsageString( USAGE Usage )
{
    static const Char* UsageStrings[4];
    static bool bUsageStringsInit = false;
    if( !bUsageStringsInit )
    {
#define INIT_USGAGE_STR(Usage)UsageStrings[Usage] = #Usage
        INIT_USGAGE_STR( USAGE_STATIC );
        INIT_USGAGE_STR( USAGE_DEFAULT );
        INIT_USGAGE_STR( USAGE_DYNAMIC );
        INIT_USGAGE_STR( USAGE_STAGING );
#undef  INIT_USGAGE_STR
        bUsageStringsInit = true;
    }
    if( Usage >= USAGE_STATIC && Usage <= USAGE_STAGING )
        return UsageStrings[Usage];
    else
    {
        UNEXPECTED("Unknow usage");
        return "Unknown usage";
    }
}

const Char* GetResourceDimString( RESOURCE_DIMENSION TexType )
{
    static const Char* TexTypeStrings[RESOURCE_DIM_NUM_DIMENSIONS];
    static bool bTexTypeStrsInit = false;
    if( !bTexTypeStrsInit )
    {
        TexTypeStrings[RESOURCE_DIM_UNDEFINED]      = "Undefined";
        TexTypeStrings[RESOURCE_DIM_BUFFER]         = "Buffer";
        TexTypeStrings[RESOURCE_DIM_TEX_1D]         = "Tex 1D";
        TexTypeStrings[RESOURCE_DIM_TEX_1D_ARRAY]   = "Tex 1D Array";
        TexTypeStrings[RESOURCE_DIM_TEX_2D]         = "Tex 2D";
        TexTypeStrings[RESOURCE_DIM_TEX_2D_ARRAY]   = "Tex 2D Array";
        TexTypeStrings[RESOURCE_DIM_TEX_3D]         = "Tex 3D";
        TexTypeStrings[RESOURCE_DIM_TEX_CUBE]       = "Tex Cube";
        TexTypeStrings[RESOURCE_DIM_TEX_CUBE_ARRAY] = "Tex Cube Array";
        static_assert(RESOURCE_DIM_NUM_DIMENSIONS == RESOURCE_DIM_TEX_CUBE_ARRAY + 1, "Not all texture type strings initialized.");

        bTexTypeStrsInit = true;
    }
    if( TexType >= RESOURCE_DIM_UNDEFINED && TexType < RESOURCE_DIM_NUM_DIMENSIONS )
        return TexTypeStrings[TexType];
    else
    {
        UNEXPECTED("Unknow texture type");
        return "Unknow texture type";
    }
}

const Char* GetBindFlagString( Uint32 BindFlag )
{
    VERIFY( (BindFlag & (BindFlag - 1)) == 0, "More than one bind flag specified" );
    switch( BindFlag )
    {
#define BIND_FLAG_STR_CASE(Flag) case Flag: return #Flag;
        BIND_FLAG_STR_CASE( BIND_VERTEX_BUFFER )
	    BIND_FLAG_STR_CASE( BIND_INDEX_BUFFER  )
	    BIND_FLAG_STR_CASE( BIND_UNIFORM_BUFFER  )
	    BIND_FLAG_STR_CASE( BIND_SHADER_RESOURCE )
	    BIND_FLAG_STR_CASE( BIND_STREAM_OUTPUT )
	    BIND_FLAG_STR_CASE( BIND_RENDER_TARGET )
	    BIND_FLAG_STR_CASE( BIND_DEPTH_STENCIL )
	    BIND_FLAG_STR_CASE( BIND_UNORDERED_ACCESS )
        BIND_FLAG_STR_CASE( BIND_INDIRECT_DRAW_ARGS )
#undef  BIND_FLAG_STR_CASE
        default: UNEXPECTED( "Unexpected bind flag ", BindFlag ); return "";
    }
}

String GetBindFlagsString( Uint32 BindFlags )
{
    if( BindFlags == 0 )
        return "0";
    String Str;
    for( Uint32 Flag = BIND_VERTEX_BUFFER; BindFlags && Flag <= BIND_INDIRECT_DRAW_ARGS; Flag <<= 1 )
    {
        if( BindFlags&Flag )
        {
            if( Str.length() )
                Str += '|';
            Str += GetBindFlagString( Flag );
            BindFlags &= ~Flag;
        }
    }
    VERIFY( BindFlags == 0, "Unknown bind flags left" );
    return Str;
}


static const Char* GetSingleCPUAccessFlagString( Uint32 CPUAccessFlag )
{
    VERIFY( (CPUAccessFlag & (CPUAccessFlag - 1)) == 0, "More than one access flag specified" );
    switch( CPUAccessFlag )
    {
#define CPU_ACCESS_FLAG_STR_CASE(Flag) case Flag: return #Flag;
        CPU_ACCESS_FLAG_STR_CASE( CPU_ACCESS_READ )
	    CPU_ACCESS_FLAG_STR_CASE( CPU_ACCESS_WRITE  )
#undef  CPU_ACCESS_FLAG_STR_CASE
        default: UNEXPECTED( "Unexpected CPU access flag ", CPUAccessFlag ); return "";
    }
}

String GetCPUAccessFlagsString( Uint32 CpuAccessFlags )
{
    if( CpuAccessFlags == 0 )
        return "0";
    String Str;
    for( Uint32 Flag = CPU_ACCESS_READ; CpuAccessFlags && Flag <= CPU_ACCESS_WRITE; Flag <<= 1 )
    {
        if( CpuAccessFlags&Flag )
        {
            if( Str.length() )
                Str += '|';
            Str += GetSingleCPUAccessFlagString( Flag );
            CpuAccessFlags &= ~Flag;
        }
    }
    VERIFY( CpuAccessFlags == 0, "Unknown CPU access flags left" );
    return Str;
}

// There is a nice standard function to_string, but there is a bug on gcc
// and it does not work
template<typename T>
String ToString( T Val )
{
    std::stringstream ss;
    ss << Val;
    return ss.str();
}

String GetTextureDescString( const TextureDesc &Desc )
{
    String Str = "Type: ";
    Str += GetResourceDimString(Desc.Type);
    Str += "; size: ";
    Str += ToString(Desc.Width);
    if( Desc.Type == RESOURCE_DIM_TEX_2D || Desc.Type == RESOURCE_DIM_TEX_2D_ARRAY || Desc.Type == RESOURCE_DIM_TEX_3D || Desc.Type == RESOURCE_DIM_TEX_CUBE || Desc.Type == RESOURCE_DIM_TEX_CUBE_ARRAY)
    {
        Str += "x";
        Str += ToString( Desc.Height );
    }

    if( Desc.Type == RESOURCE_DIM_TEX_3D )
    {
        Str += "x";
        Str += ToString( Desc.Depth );
    }
    
    if( Desc.Type == RESOURCE_DIM_TEX_1D_ARRAY || Desc.Type == RESOURCE_DIM_TEX_2D_ARRAY || Desc.Type == RESOURCE_DIM_TEX_CUBE || Desc.Type == RESOURCE_DIM_TEX_CUBE_ARRAY )
    {
        Str += "; Num Slices: ";
        Str += ToString( Desc.ArraySize );
    }

    auto FmtName = GetTextureFormatAttribs( Desc.Format ).Name;
    Str += "; Format: ";
    Str += FmtName;

    Str += "; Mip levels: ";
    Str += ToString( Desc.MipLevels );

    Str += "; Sample Count: ";
    Str += ToString( Desc.SampleCount );

    Str += "; Usage: ";
    Str += GetUsageString(Desc.Usage);

    Str += "; Bind Flags: ";
    Str += GetBindFlagsString(Desc.BindFlags);

    Str += "; CPU access: ";
    Str += GetCPUAccessFlagsString(Desc.CPUAccessFlags);

    return Str;
}

const Char* GetBufferModeString( BUFFER_MODE Mode )
{
    static const Char* BufferModeStrings[BUFFER_MODE_NUM_MODES];
    static bool bBuffModeStringsInit = false;
    if( !bBuffModeStringsInit )
    {
#define INIT_BUFF_MODE_STR(Mode)BufferModeStrings[Mode] = #Mode
        INIT_BUFF_MODE_STR( BUFFER_MODE_UNDEFINED );
        INIT_BUFF_MODE_STR( BUFFER_MODE_FORMATTED );
        INIT_BUFF_MODE_STR( BUFFER_MODE_STRUCTURED );
        INIT_BUFF_MODE_STR( BUFFER_MODE_RAW );
#undef  INIT_BUFF_MODE_STR
        static_assert(BUFFER_MODE_NUM_MODES == BUFFER_MODE_RAW + 1, "Not all buffer mode strings initialized.");
        bBuffModeStringsInit = true;
    }
    if( Mode >= BUFFER_MODE_UNDEFINED && Mode < BUFFER_MODE_NUM_MODES )
        return BufferModeStrings[Mode];
    else
    {
        UNEXPECTED( "Unknown buffer mode" );
        return "Unknown buffer mode";
    }
}

String GetBufferFormatString(const BufferFormat& Fmt)
{
    String Str;
    Str += GetValueTypeString( Fmt.ValueType );
    if( Fmt.IsNormalized )
        Str += " norm";
    Str += " x ";
    Str += ToString(Uint32{Fmt.NumComponents});
    return Str;
}

String GetBufferDescString( const BufferDesc &Desc )
{
    String Str;
    Str += "Size: ";
    bool bIsLarge = false;
    if( Desc.uiSizeInBytes > (1 << 20) )
    {
        Str += ToString( Desc.uiSizeInBytes / (1<<20) );
        Str += " Mb (";
        bIsLarge = true;
    }
    else if( Desc.uiSizeInBytes > (1 << 10) )
    {
        Str += ToString( Desc.uiSizeInBytes / (1<<10) );
        Str += " Kb (";
        bIsLarge = true;
    }

    Str += ToString( Desc.uiSizeInBytes );
    Str += " bytes";
    if( bIsLarge )
        Str += ')';
    
    Str += "; Mode: ";
    Str += GetBufferModeString(Desc.Mode);

    Str += "; Usage: ";
    Str += GetUsageString(Desc.Usage);

    Str += "; Bind Flags: ";
    Str += GetBindFlagsString(Desc.BindFlags);

    Str += "; CPU access: ";
    Str += GetCPUAccessFlagsString(Desc.CPUAccessFlags);

    Str += "; stride: ";
    Str += ToString( Desc.ElementByteStride );
    Str += " bytes";

    return Str;
}

const Char* GetResourceStateFlagString( RESOURCE_STATE State )
{
    VERIFY((State & (State-1)) == 0, "Single state is expected");
    switch(State)
    {
        case RESOURCE_STATE_UNKNOWN:           return "UNKNOWN";
        case RESOURCE_STATE_UNDEFINED:         return "UNDEFINED";
        case RESOURCE_STATE_VERTEX_BUFFER:     return "VERTEX_BUFFER";
        case RESOURCE_STATE_CONSTANT_BUFFER:   return "CONSTANT_BUFFER";
        case RESOURCE_STATE_INDEX_BUFFER:      return "INDEX_BUFFER";
        case RESOURCE_STATE_RENDER_TARGET:     return "RENDER_TARGET";
        case RESOURCE_STATE_UNORDERED_ACCESS:  return "UNORDERED_ACCESS";
        case RESOURCE_STATE_DEPTH_WRITE:       return "DEPTH_WRITE";
        case RESOURCE_STATE_DEPTH_READ:        return "DEPTH_READ";
        case RESOURCE_STATE_SHADER_RESOURCE:   return "SHADER_RESOURCE";
        case RESOURCE_STATE_STREAM_OUT:        return "STREAM_OUT";
        case RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
        case RESOURCE_STATE_COPY_DEST:         return "COPY_DEST";
        case RESOURCE_STATE_COPY_SOURCE:       return "COPY_SOURCE";
        case RESOURCE_STATE_RESOLVE_DEST:      return "RESOLVE_DEST";
        case RESOURCE_STATE_RESOLVE_SOURCE:    return "RESOLVE_SOURCE";
        case RESOURCE_STATE_PRESENT:           return "PRESENT";
        default:
            UNEXPECTED("Unknown resource state");
            return "UNKNOWN";
    }
}

String GetResourceStateString( RESOURCE_STATE State )
{
    if(State == RESOURCE_STATE_UNKNOWN)
        return "UNKNOWN";

    String str;
    while (State != 0)
    {
        if (!str.empty())
            str.push_back('|');
        
        auto lsb = State & ~(State-1);
        const auto* StateFlagString = GetResourceStateFlagString(static_cast<RESOURCE_STATE>(lsb));
        str.append(StateFlagString);
        State = static_cast<RESOURCE_STATE>(State & ~lsb);
    }
    return str;
}

Uint32 ComputeMipLevelsCount( Uint32 Width )
{
    if (Width == 0)
        return 0;

    Uint32 MipLevels = 0;
    while( (Width >> MipLevels) > 0 )
        ++MipLevels;
    VERIFY( Width >= (1U << (MipLevels-1)) && Width < (1U << MipLevels), "Incorrect number of Mip levels" );
    return MipLevels;
}

Uint32 ComputeMipLevelsCount( Uint32 Width, Uint32 Height )
{
    return ComputeMipLevelsCount( std::max( Width, Height ) );
}

Uint32 ComputeMipLevelsCount( Uint32 Width, Uint32 Height, Uint32 Depth )
{
    return ComputeMipLevelsCount( std::max(std::max( Width, Height ), Depth) );
}

bool VerifyResourceStates(RESOURCE_STATE State, bool IsTexture)
{
    static_assert(RESOURCE_STATE_MAX_BIT == 0x8000, "Please update this function to handle the new resource state");

#define VERIFY_EXCLUSIVE_STATE(ExclusiveState)\
if ( (State & ExclusiveState) != 0 && (State & ~ExclusiveState) != 0 )\
{\
    LOG_ERROR_MESSAGE("State ", GetResourceStateString(State), " is invalid: " #ExclusiveState " can't be combined with any other state");\
    return false;\
}

    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_UNDEFINED);
    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_UNORDERED_ACCESS);
    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_RENDER_TARGET);
    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_DEPTH_WRITE);
    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_COPY_DEST);
    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_RESOLVE_DEST);
    VERIFY_EXCLUSIVE_STATE(RESOURCE_STATE_PRESENT);
#undef VERIFY_EXCLUSIVE_STATE

    if (IsTexture)
    {
        if (State &
            (RESOURCE_STATE_VERTEX_BUFFER   |
             RESOURCE_STATE_CONSTANT_BUFFER |
             RESOURCE_STATE_INDEX_BUFFER    |
             RESOURCE_STATE_STREAM_OUT      |
             RESOURCE_STATE_INDIRECT_ARGUMENT))
        {
            LOG_ERROR_MESSAGE("State ", GetResourceStateString(State), " is invalid: states RESOURCE_STATE_VERTEX_BUFFER, "
                              "RESOURCE_STATE_CONSTANT_BUFFER, RESOURCE_STATE_INDEX_BUFFER, RESOURCE_STATE_STREAM_OUT, "
                              "RESOURCE_STATE_INDIRECT_ARGUMENT are not applicable to a texture");
            return false;
        }
    }
    else
    {
        if (State &
            (RESOURCE_STATE_RENDER_TARGET  |
             RESOURCE_STATE_DEPTH_WRITE    |
             RESOURCE_STATE_DEPTH_READ     |
             RESOURCE_STATE_RESOLVE_SOURCE |
             RESOURCE_STATE_RESOLVE_DEST   |
             RESOURCE_STATE_PRESENT))
        {
            LOG_ERROR_MESSAGE("State ", GetResourceStateString(State), " is invalid: states RESOURCE_STATE_RENDER_TARGET, "
                              "RESOURCE_STATE_DEPTH_WRITE, RESOURCE_STATE_DEPTH_READ, RESOURCE_STATE_RESOLVE_SOURCE, "
                              "RESOURCE_STATE_RESOLVE_DEST, RESOURCE_STATE_PRESENT are not applicable to a buffer");
            return false;

        }
    }
    
    return true;
}

MipLevelProperties GetMipLevelProperties(const TextureDesc& TexDesc, Uint32 MipLevel)
{
    MipLevelProperties MipProps;
    const auto& FmtAttribs = GetTextureFormatAttribs(TexDesc.Format);

    MipProps.LogicalWidth  = std::max(TexDesc.Width  >> MipLevel, 1u);
    MipProps.LogicalHeight = std::max(TexDesc.Height >> MipLevel, 1u);
    MipProps.Depth  = (TexDesc.Type == RESOURCE_DIM_TEX_3D) ? std::max(TexDesc.Depth >> MipLevel, 1u) : 1u;
    if (FmtAttribs.ComponentType == COMPONENT_TYPE_COMPRESSED)
    {
        VERIFY_EXPR(FmtAttribs.BlockWidth > 1 && FmtAttribs.BlockHeight > 1);
        VERIFY((FmtAttribs.BlockWidth  & (FmtAttribs.BlockWidth-1))  == 0, "Compressed block width is expected to be power of 2");
        VERIFY((FmtAttribs.BlockHeight & (FmtAttribs.BlockHeight-1)) == 0, "Compressed block height is expected to be power of 2");
        // For block-compression formats, all parameters are still specified in texels rather than compressed texel blocks (18.4.1)
        MipProps.StorageWidth   = Align(MipProps.LogicalWidth, Uint32{FmtAttribs.BlockWidth});
        MipProps.StorageHeight  = Align(MipProps.LogicalHeight,Uint32{FmtAttribs.BlockHeight});
        MipProps.RowSize        = MipProps.StorageWidth  / Uint32{FmtAttribs.BlockWidth} * Uint32{FmtAttribs.ComponentSize}; // ComponentSize is the block size
        MipProps.DepthSliceSize = MipProps.StorageHeight / Uint32{FmtAttribs.BlockHeight} * MipProps.RowSize;
        MipProps.MipSize        = MipProps.DepthSliceSize * MipProps.Depth;
    }
    else
    {
        MipProps.StorageWidth   = MipProps.LogicalWidth;
        MipProps.StorageHeight  = MipProps.LogicalHeight;
        MipProps.RowSize        = MipProps.StorageWidth * Uint32{FmtAttribs.ComponentSize} * Uint32{FmtAttribs.NumComponents};
        MipProps.DepthSliceSize = MipProps.RowSize * MipProps.StorageHeight;
        MipProps.MipSize        = MipProps.DepthSliceSize * MipProps.Depth;
    }

    return MipProps;
}

}
