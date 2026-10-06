//
// Created by HP on 2026/6/30.
//
#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "faa-core/win_utils/faa_win_utils.h"
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.Graphics.Capture.Interop.h>

// faa::core::cap::wgc::FAAWindowCapture::FAAWindowCapture(){};
using faa::core::cap::wgc::FAAWindowCapture;

 FAAWindowCapture&  FAAWindowCapture::initD3d11(D3DDeviceContext device_context) {


    try {
        HRESULT hr = D3D11CreateDevice(
            nullptr,                                     // 使用默认适配器
            D3D_DRIVER_TYPE_HARDWARE,                   // 硬件驱动
            nullptr,                                    // 无软件驱动
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,           // 创建标志
            nullptr, 0,                   // 使用默认功能级别
            D3D11_SDK_VERSION,
            device_context.device.put(),
            nullptr,                         // 不需要返回实际功能级别
            device_context.context.put()
        );
        if (FAILED(hr)) {
            this->log.error("D3D11CreateDevice failed: {}", hr);
            return *this;
        }
        this->log.info("D3D11CreateDevice success");
        winrt::com_ptr<IDXGIDevice> dxgiDevice = device_context.device.as<IDXGIDevice>();
        if (dxgiDevice==nullptr){
            this->log.error("dxgi device query failed");
            return *this;
        }

        winrt::com_ptr<::IInspectable> inspectable;
        hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put());

        if (FAILED(hr)) {
            this->log.error("CreateDirect3D11DeviceFromDXGIDevice failed: {}", hr);
            return *this;
        }

        device_context.winrtDevice =
            inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();

        if (device_context.winrtDevice ==nullptr) {
            this->log.error("winrt device query failed");
            return *this;
        }
        this->log.info("CreateDirect3D11DeviceFromDXGIDevice success");
        winrt::com_ptr<IDXGIDevice> dxgi_gpu = device_context.device.as<IDXGIDevice>();
        winrt::com_ptr<IDXGIAdapter> adapter;
        hr = dxgiDevice->GetAdapter(adapter.put());

        if (SUCCEEDED(hr)){
            this->log.info("GetAdapter success");
        }
        this->d3d_device_context=device_context;
    }catch (std::exception& e){
        this->log.error("init_d3d11 failed: {}", e.what());
    }
    return *this;
}

FAAWindowCapture& FAAWindowCapture::initWgcRtCtx(WGCRunTimeContext run_time_ctx)
{
     this->log.debug("initWgcRtCtx for window: {}", this->target_window_name);
     this->target_window_handle=win_utils::GetWindowHandle(this->target_window_name);
     if (this->target_window_handle==nullptr){
         this->log.debug("try find to windows by class");
         this->target_window_handle=win_utils::GetWindowHandleByClass(this->target_window_name);
     }
     if (this->target_window_handle==nullptr){
         this->log.debug("try find to windows by name");
         this->target_window_handle=win_utils::GetWindowHandleByName(this->target_window_name);
     }
     if (this->target_window_handle==nullptr){
         this->log.error("this target_window_handle query failed ");
         this->is_init_=false;
         return *this;
     }
     this->is_init_=true;

     auto interopFactory = winrt::get_activation_factory<
             winrt::Windows::Graphics::Capture::GraphicsCaptureItem,
             IGraphicsCaptureItemInterop
         >();
     auto hr = interopFactory-> CreateForWindow(
             this->target_window_handle,
             winrt::guid_of<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>(),
             winrt::put_abi(run_time_ctx.captureItem)
     );
     if (FAILED(hr)) {
         this->log.error("CreateForWindow failed: {}", hr);
         this->is_init_=false;
         return *this;
     }
     this->log.debug("CreateForWindow success");
     run_time_ctx.framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
           this->d3d_device_context.winrtDevice,
           winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
           2, // 双缓冲
           run_time_ctx.captureItem.Size()
     );
     this->run_time_ctx=run_time_ctx;
     return *this;
}

FAAWindowCapture& FAAWindowCapture::startSession(){
     this->log.debug("startSession");
     if (this->is_init()==false) {
         this->log.error("is_init failed");
         return *this;
     }
     if (this->run_time_ctx.framePool==nullptr){
         this->log.error("run_time_ctx.framePool is nullptr");
         return  *this;
     }

     this->run_time_ctx.session= this->run_time_ctx.framePool.CreateCaptureSession(this->run_time_ctx.captureItem);
     this->run_time_ctx.session.StartCapture();
     this->log.debug("startSession success");
     return *this;
     // }
}

FAAWindowCapture& FAAWindowCapture::getFrame(WgcTextureData& texture_data, cv::Mat& mat)
{
     // this->log.debug("getFrame");
     if (auto frame = run_time_ctx.framePool.TryGetNextFrame()){
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
             this->log.error("GetInterface failed: {}", hr);
             return *this;
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

         hr = this->d3d_device_context.device->CreateTexture2D(&stagingDesc, nullptr, stagingTexture.put());

         if (FAILED(hr)) {
             this->log.error("CreateTexture2D failed: {}", hr);
             return *this;
         }

         this->d3d_device_context.context->CopyResource(stagingTexture.get(), capturedTexture.get());
         D3D11_MAPPED_SUBRESOURCE mappedResource;
         hr = this->d3d_device_context.context->Map(
             stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mappedResource);
         if (FAILED(hr)) {
             this->log.error("Map failed: {}", hr);
             return *this;
         }

         texture_data.data = static_cast<uint8_t*>(mappedResource.pData);
         texture_data.width = capturedDesc.Width;
         texture_data.height = capturedDesc.Height;
         texture_data.RowPitch = mappedResource.RowPitch;
         texture_data.DepthPitch = mappedResource.DepthPitch;

         this->d3d_device_context.context->Unmap(stagingTexture.get(), 0);
         texture_data.textureForMat(mat);
         return *this;
     }
     return *this;
}
