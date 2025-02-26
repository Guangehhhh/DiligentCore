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

#include "BaseInterfacesGL.h"
#include "ShaderGL.h"
#include "ShaderBase.h"
#include "RenderDevice.h"
#include "GLObjectWrapper.h"
#include "RenderDeviceGLImpl.h"
#include "GLProgramResources.h"

namespace Diligent
{

class FixedBlockMemoryAllocator;

inline GLenum GetGLShaderType(SHADER_TYPE ShaderType)
{
    switch(ShaderType)
    {
        case SHADER_TYPE_VERTEX:    return GL_VERTEX_SHADER;          break;
        case SHADER_TYPE_PIXEL:     return GL_FRAGMENT_SHADER;        break;
        case SHADER_TYPE_GEOMETRY:  return GL_GEOMETRY_SHADER;        break;
        case SHADER_TYPE_HULL:      return GL_TESS_CONTROL_SHADER;    break;
        case SHADER_TYPE_DOMAIN:    return GL_TESS_EVALUATION_SHADER; break;
        case SHADER_TYPE_COMPUTE:   return GL_COMPUTE_SHADER;         break;
        default: return 0;
    }
}

inline GLenum ShaderTypeToGLShaderBit(SHADER_TYPE ShaderType)
{
    switch(ShaderType)
    {
        case SHADER_TYPE_VERTEX:    return GL_VERTEX_SHADER_BIT;          break;
        case SHADER_TYPE_PIXEL:     return GL_FRAGMENT_SHADER_BIT;        break;
        case SHADER_TYPE_GEOMETRY:  return GL_GEOMETRY_SHADER_BIT;        break;
        case SHADER_TYPE_HULL:      return GL_TESS_CONTROL_SHADER_BIT;    break;
        case SHADER_TYPE_DOMAIN:    return GL_TESS_EVALUATION_SHADER_BIT; break;
        case SHADER_TYPE_COMPUTE:   return GL_COMPUTE_SHADER_BIT;         break;
        default: return 0;
    }
}

/// Implementation of the Diligent::IShaderGL interface
class ShaderGLImpl final : public ShaderBase<IShaderGL, RenderDeviceGLImpl>
{
public:
    using TShaderBase = ShaderBase<IShaderGL, RenderDeviceGLImpl>;

    ShaderGLImpl(IReferenceCounters*        pRefCounters,
                 RenderDeviceGLImpl*        pDeviceGL,
                 const ShaderCreateInfo&    ShaderCreateInfo,
                 bool                       bIsDeviceInternal = false);
    ~ShaderGLImpl();

    virtual void QueryInterface(const INTERFACE_ID& IID, IObject** ppInterface)override final;

    virtual Uint32 GetResourceCount()const override final;
    virtual ShaderResourceDesc GetResource(Uint32 Index)const override final;

    static GLObjectWrappers::GLProgramObj LinkProgram(IShader** ppShaders, Uint32 NumShaders, bool IsSeparableProgram);

private:
    GLObjectWrappers::GLShaderObj m_GLShaderObj;
    GLProgramResources            m_Resources;
};

}
