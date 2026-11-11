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

FaaWindowCapture& FaaWindowCapture::initD3d11() {

    #if defined(_WIN32)

    try  {
        if (!core_ptr) {
            if (logger) logger->error("core_ptr is null");
            throw std::runtime_error("core_ptr is null");
        }
        HRESULT hr = D3D11CreateDevice(
           nullptr,                                     // 使用默认适配器
           D3D_DRIVER_TYPE_HARDWARE,                   // 硬件驱动
           nullptr,                                    // 无软件驱动
           D3D11_CREATE_DEVICE_BGRA_SUPPORT,           // 创建标志
           nullptr, 0,                   // 使用默认功能级别
           D3D11_SDK_VERSION,
           core_ptr->d3d_device_context.device.put(),
           nullptr,                         // 不需要返回实际功能级别
           core_ptr->d3d_device_context.context.put()
       );

       if (FAILED(hr)) {
           if (logger) logger->error("D3D11CreateDevice failed: {}", hr);
           return *this;
       }
        winrt::com_ptr<IDXGIDevice> dxgiDevice =
            core_ptr->d3d_device_context.device.as<IDXGIDevice>();
        if (!dxgiDevice){
            if (logger) logger->error("dxgi device query failed");
            return *this;
        }

        winrt::com_ptr<IInspectable> inspectable;
        hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put());

        if (FAILED(hr)) {
            if (logger) logger->error("CreateDirect3D11DeviceFromDXGIDevice failed: {}", hr);
            return *this;
        }

        core_ptr->d3d_device_context.winrtDevice =
            inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();

        if ( core_ptr->d3d_device_context.winrtDevice ==nullptr) {
            if (logger) logger->error("winrt device query failed");
            return *this;
        }
        if (logger) logger->info("CreateDirect3D11DeviceFromDXGIDevice success");
        winrt::com_ptr<IDXGIDevice> dxgi_gpu =  core_ptr->d3d_device_context.device.as<IDXGIDevice>();
        winrt::com_ptr<IDXGIAdapter> adapter;
        hr = dxgiDevice->GetAdapter(adapter.put());
        DXGI_ADAPTER_DESC desc{};
        adapter->GetDesc(&desc);

        if (SUCCEEDED(hr)){
            if (logger) logger->info("GetAdapter success");
            if (logger) logger->debug("GPU: {}", winrt::to_string(desc.Description));

        }

    }catch (std::exception& e){
        if (logger) logger->error("init d3d error: {}", e.what());
    }
    #else
            throw std::runtime_error("not implemented");
    #   endif
    return *this;
}

FaaWindowCapture& FaaWindowCapture::initWgcRtCtx() {

    if (const auto isFind = this->core_ptr->find(this->logger); !isFind) {
        if (logger) logger->error("not found window");
        return *this;
    }

    const auto interopFactory = winrt::get_activation_factory<
           winrt::Windows::Graphics::Capture::GraphicsCaptureItem,
           IGraphicsCaptureItemInterop
       >();

    const auto hr = interopFactory-> CreateForWindow(
            core_ptr->handle,
            winrt::guid_of<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>(),
            winrt::put_abi(
                core_ptr->run_time_ctx.captureItem
            )
    );
    if (FAILED(hr)) {
        if (logger) logger->error("CreateForWindow failed: {}", hr);
        return *this;
    }
    if (logger) logger->debug("CreateForWindow success");
    core_ptr->run_time_ctx.

    framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
         core_ptr->d3d_device_context.winrtDevice,
          winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
          2, // 双缓冲
          core_ptr->run_time_ctx.captureItem.Size()
    );
    this->isInitialized = true;

    return *this;
}