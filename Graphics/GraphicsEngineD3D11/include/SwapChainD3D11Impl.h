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
/// Declaration of Diligent::SwapChainD3D11Impl class

#include "SwapChainD3D11.h"
#include "SwapChainD3DBase.h"

namespace Diligent
{

class IMemoryAllocator;
/// Implementation of the Diligent::ISwapChainD3D11 interface
class SwapChainD3D11Impl final : public SwapChainD3DBase<ISwapChainD3D11, IDXGISwapChain>
{
public:
    using TSwapChainBase = SwapChainD3DBase<ISwapChainD3D11, IDXGISwapChain>;

    SwapChainD3D11Impl(IReferenceCounters*           pRefCounters,
                       const SwapChainDesc&          SCDesc, 
                       const FullScreenModeDesc&     FSDesc,
                       class RenderDeviceD3D11Impl*  pRenderDeviceD3D11,
                       class DeviceContextD3D11Impl* pDeviceContextD3D11,
                       void*                         pNativeWndHandle);
    ~SwapChainD3D11Impl();

    virtual void QueryInterface(const INTERFACE_ID& IID, IObject** ppInterface)override final;

    virtual void Present(Uint32 SyncInterval)override final;
    virtual void Resize( Uint32 NewWidth, Uint32 NewHeight )override final;

    virtual IDXGISwapChain* GetDXGISwapChain()override final{ return m_pSwapChain; }

    virtual ITextureViewD3D11* GetCurrentBackBufferRTV()override final{return m_pRenderTargetView;}
    virtual ITextureViewD3D11* GetDepthBufferDSV()      override final{return m_pDepthStencilView;}

private:
    virtual void UpdateSwapChain(bool CreateNew)override final;
    void CreateRTVandDSV();

    RefCntAutoPtr<ITextureViewD3D11> m_pRenderTargetView;
    RefCntAutoPtr<ITextureViewD3D11> m_pDepthStencilView;
};

}
