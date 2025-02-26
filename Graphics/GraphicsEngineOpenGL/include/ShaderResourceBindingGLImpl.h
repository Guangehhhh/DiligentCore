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
/// Declaration of Diligent::ShaderResourceBindingGLImpl class

#include "ShaderResourceBindingGL.h"
#include "RenderDeviceGL.h"
#include "ShaderResourceBindingBase.h"
#include "GLProgramResources.h"
#include "ShaderBase.h"
#include "GLProgramResourceCache.h"
#include "GLPipelineResourceLayout.h"

namespace Diligent
{

class FixedBlockMemoryAllocator;
class PipelineStateGLImpl;

/// Implementation of the Diligent::IShaderResourceBindingGL interface
class ShaderResourceBindingGLImpl final : public ShaderResourceBindingBase<IShaderResourceBindingGL>
{
public:
    using TBase = ShaderResourceBindingBase<IShaderResourceBindingGL>;

    ShaderResourceBindingGLImpl(IReferenceCounters*     pRefCounters,
                                PipelineStateGLImpl*    pPSO,
                                GLProgramResources*     ProgramResources,
                                Uint32                  NumPrograms);
    ~ShaderResourceBindingGLImpl();

    virtual void QueryInterface( const INTERFACE_ID& IID, IObject** ppInterface )override final;

    virtual void BindResources(Uint32 ShaderFlags, IResourceMapping* pResMapping, Uint32 Flags)override final;

    virtual IShaderResourceVariable* GetVariableByName(SHADER_TYPE ShaderType, const char *Name)override final;

    virtual Uint32 GetVariableCount(SHADER_TYPE ShaderType) const override final;

    virtual IShaderResourceVariable* GetVariableByIndex(SHADER_TYPE ShaderType, Uint32 Index)override final;

    virtual void InitializeStaticResources(const IPipelineState* pPipelineState)override final;

    const GLProgramResourceCache& GetResourceCache(PipelineStateGLImpl* pdbgPSO);

private:
    // The resource layout only references mutable and dynamic variables
    GLPipelineResourceLayout m_ResourceLayout;

    // The resource cache holds resource bindings for all variables
    GLProgramResourceCache   m_ResourceCache;

    bool                     m_bIsStaticResourcesBound = false;
};

}
