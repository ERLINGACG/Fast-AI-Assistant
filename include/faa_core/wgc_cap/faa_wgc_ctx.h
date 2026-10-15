//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_WGC_CTX_H
#define FASTSCREEN_FAA_WGC_CTX_H
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <d3d11.h>
namespace faa::core::cap::wgc{

    struct D3DDeviceContext {
        winrt::com_ptr<ID3D11Device> device;
        winrt::com_ptr<ID3D11DeviceContext> context;
        winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice winrtDevice;
    };

    struct WGCRunTimeContext {
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem{nullptr};
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool{nullptr};
        winrt::Windows::Graphics::Capture::GraphicsCaptureSession session{nullptr};
        HMONITOR monitor{};
    };
}
#endif //FASTSCREEN_FAA_WGC_CTX_H