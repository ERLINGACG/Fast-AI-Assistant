//
// Created by HP on 2026/11/11.
//

#include "faa_core/cap/wgc_cap/faa_wgc_cap.h"
#include "faa_core/win_utils/faa_win_utils.h"
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.Graphics.Capture.Interop.h>

#include <utility>
#include "faa_core/cap/wgc_cap/private/faa_wgc_cap_impi.h"

using faa::core::cap::wgc::FaaWindowCapture;
using faa::core::cap::wgc::WgcCapCore;
bool FaaWindowCapture::getFrame(WgcTextureData& texture_data, frame::FaaMat& mat) {

    auto* log = logger;

     if (auto frame = core_ptr->run_time_ctx.framePool.TryGetNextFrame()){
         auto surface = frame.Surface();
         winrt::com_ptr<ID3D11Texture2D> capturedTexture;
         const auto access = surface.as<
             Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess
         >();
         auto hr = access->GetInterface(
             winrt::guid_of<ID3D11Texture2D>(),
             winrt::put_abi(capturedTexture)
         );
         if (FAILED(hr)){
             if (log) log->error("GetInterface failed: {}", hr);
             return false;
         }
         D3D11_TEXTURE2D_DESC capturedDesc;
         capturedTexture->GetDesc(&capturedDesc);
         // 创建 staging 纹理
         D3D11_TEXTURE2D_DESC stagingDesc = capturedDesc;
         stagingDesc.Usage = D3D11_USAGE_STAGING;
         stagingDesc.BindFlags = 0;
         stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
         stagingDesc.MiscFlags = 0;
         winrt::com_ptr<ID3D11Texture2D> stagingTexture;

         hr = core_ptr->d3d_device_context.device->CreateTexture2D(&stagingDesc, nullptr, stagingTexture.put());

         if (FAILED(hr)) {
             if (log) log->error("CreateTexture2D failed: {}", hr);
             return false;
         }

         core_ptr->d3d_device_context.context->CopyResource(stagingTexture.get(), capturedTexture.get());
         D3D11_MAPPED_SUBRESOURCE mappedResource;
         hr = core_ptr->d3d_device_context.context->Map(
             stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mappedResource);
         if (FAILED(hr)) {
             if (log) log->error("Map failed: {}", hr);
             return false;
         }

         texture_data.data = static_cast<uint8_t*>(mappedResource.pData);
         texture_data.width = capturedDesc.Width;
         texture_data.height = capturedDesc.Height;
         texture_data.RowPitch = mappedResource.RowPitch;
         texture_data.DepthPitch = mappedResource.DepthPitch;

         core_ptr->d3d_device_context.context->Unmap(stagingTexture.get(), 0);
         texture_data.textureForMat(mat);
         return true;
     }
     return false;
}

bool FaaWindowCapture::open(common::ilogger::FaaIlogger* logger) {
    this->setLogger(logger).
       initD3d11().
       initWgcRtCtx().
       startSession();
    return this->isInitialized;
}

bool FaaWindowCapture::read(frame::FaaMat& mat) {

    WgcTextureData texture_data{};
    return this->getFrame(texture_data, mat);

}