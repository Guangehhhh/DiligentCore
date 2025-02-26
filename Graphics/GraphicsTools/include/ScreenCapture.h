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

#include <mutex>
#include <vector>
#include <deque>

#include "../../GraphicsEngine/interface/SwapChain.h"
#include "../../GraphicsEngine/interface/RenderDevice.h"
#include "../../GraphicsEngine/interface/DeviceContext.h"
#include "../../../Common/interface/RefCntAutoPtr.h"

namespace Diligent
{
    
class ScreenCapture
{
public:
    ScreenCapture(IRenderDevice* pDevice);

    void Capture(ISwapChain* pSwapChain, IDeviceContext* pContext, Uint32 FrameId);
        
    struct CaptureInfo
    {
        RefCntAutoPtr<ITexture> pTexture;
        Uint32                  Id      = 0;

        operator bool() const
        {
            return pTexture != nullptr;
        }
    };

    CaptureInfo GetCapture();
    void RecycleStagingTexture(RefCntAutoPtr<ITexture>&& pTexture);

    size_t GetNumPendingCaptures()
    {
        std::lock_guard<std::mutex> Lock{m_PendingTexturesMtx};
        return m_PendingTextures.size();
    }

private:
    RefCntAutoPtr<IFence>        m_pFence;
    RefCntAutoPtr<IRenderDevice> m_pDevice;

    std::mutex                           m_AvailableTexturesMtx;
    std::vector<RefCntAutoPtr<ITexture>> m_AvailableTextures;

    std::mutex                                             m_PendingTexturesMtx;
    struct PendingTextureInfo
    {
        PendingTextureInfo(RefCntAutoPtr<ITexture>&& _pTex, Uint32 _Id, Uint64 _Fence) :
            pTex    (std::move(_pTex)),
            Id      (_Id),
            Fence   (_Fence)
        {
        }

        RefCntAutoPtr<ITexture> pTex;
        const Uint32            Id;
        const Uint64            Fence;
    };
    std::deque<PendingTextureInfo> m_PendingTextures;

    Uint64 m_CurrentFenceValue = 1;
};

}
