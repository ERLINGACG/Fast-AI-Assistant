//
// Created by HP on 2026/6/30.
//
#include "faa_core/wgc_cap/faa_wgc_cap.h"
#include "faa_core/win_utils/faa_win_utils.h"
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.Graphics.Capture.Interop.h>

#include <utility>
#include "faa_core/wgc_cap/private/faa_wgc_cap_impi.h"

using faa::core::cap::wgc::FAAWindowCapture;
using faa::core::cap::wgc::WgcCapCore;


FAAWindowCapture::FAAWindowCapture(std::string target_window_name, const info::log::Ilog log) : log(log)
{

    this->core_ptr = std::make_unique<WgcCapCore>();
    this->core_ptr->target = std::move(target_window_name);
}

FAAWindowCapture::~FAAWindowCapture() = default;
FAAWindowCapture::FAAWindowCapture(FAAWindowCapture&&) noexcept = default;
FAAWindowCapture& FAAWindowCapture::operator=(FAAWindowCapture&&) noexcept = default;


void FAAWindowCapture::operator >> (frame::FaaMat& mat)
{
    this->read(mat);
}

FAAWindowCapture& FAAWindowCapture::initD3d11() {

    #if defined(_WIN32)
    auto* log = core_ptr->log;
    try  {
        if (!core_ptr) {
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
           if (log) log->error("D3D11CreateDevice failed: {}", hr);
           return *this;
       }
        winrt::com_ptr<IDXGIDevice> dxgiDevice =
            core_ptr->d3d_device_context.device.as<IDXGIDevice>();
        if (!dxgiDevice){
            this->log.error("dxgi device query failed");
            this->core_ptr->is_init_ = false;
            return *this;
        }

        winrt::com_ptr<::IInspectable> inspectable;
        hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put());

        if (FAILED(hr)) {
            this->log.error("CreateDirect3D11DeviceFromDXGIDevice failed: {}", hr);
            this->core_ptr->is_init_ = false;
            return *this;
        }

        core_ptr->d3d_device_context.winrtDevice =
            inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();

        if ( core_ptr->d3d_device_context.winrtDevice ==nullptr) {
            this->log.error("winrt device query failed");
            this->core_ptr->is_init_ = false;
            return *this;
        }
        this->log.info("CreateDirect3D11DeviceFromDXGIDevice success");
        winrt::com_ptr<IDXGIDevice> dxgi_gpu =  core_ptr->d3d_device_context.device.as<IDXGIDevice>();
        winrt::com_ptr<IDXGIAdapter> adapter;
        hr = dxgiDevice->GetAdapter(adapter.put());
        DXGI_ADAPTER_DESC desc{};
        adapter->GetDesc(&desc);

        if (SUCCEEDED(hr)){
            if (log) log->info("GetAdapter success");
            if (log) log->debug("GPU: {}", winrt::to_string(desc.Description));
            this->core_ptr->is_init_ = true;
        }

    }catch (std::exception& e){
        if (log) log->info("init d3d error: {}", e.what());
    }
    #else
            throw std::runtime_error("not implemented");
    #   endif
    return *this;
}

FAAWindowCapture& FAAWindowCapture::initWgcRtCtx() {

    auto* log = core_ptr->log;
    if (const auto isFind = this->core_ptr->find(); !isFind) {
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
        if (log) log->error("CreateForWindow failed: {}", hr);
        core_ptr->is_init_ = false;
        return *this;
    }
    if (log) log->debug("CreateForWindow success");
    core_ptr->run_time_ctx.

    framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
         core_ptr->d3d_device_context.winrtDevice,
          winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
          2, // 双缓冲
          core_ptr->run_time_ctx.captureItem.Size()
    );
    this->core_ptr->is_init_ = true;

    return *this;
}

FAAWindowCapture& FAAWindowCapture::setLogger(info::log::Ilog* log) {
    this->core_ptr->log=log;
    return *this;
}

FAAWindowCapture& FAAWindowCapture::startSession(){
    auto* log = core_ptr->log;
    auto& ctx = core_ptr->run_time_ctx;
    if (log) log->debug("startSession");

    if (!core_ptr->isSuccess()) {
        if (log) log->error("is_init failed");
        return *this;
    }
    if (!ctx.framePool) {
        if (log) log->error("run_time_ctx.framePool is nullptr");
        return *this;
    }

    ctx.session = ctx.framePool.CreateCaptureSession(ctx.captureItem);
    ctx.session.StartCapture();
    if (log) log->debug("startSession success");
    return *this;

}


bool FAAWindowCapture::getFrame(WgcTextureData& texture_data, frame::FaaMat& mat)
{

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
             this->log.error("GetInterface failed: {}", hr);
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
             this->log.error("CreateTexture2D failed: {}", hr);
             return false;
         }

         core_ptr->d3d_device_context.context->CopyResource(stagingTexture.get(), capturedTexture.get());
         D3D11_MAPPED_SUBRESOURCE mappedResource;
         hr = core_ptr->d3d_device_context.context->Map(
             stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mappedResource);
         if (FAILED(hr)) {
             this->log.error("Map failed: {}", hr);
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

bool FAAWindowCapture::open(info::log::Ilog* log) {
    this->setLogger(log).
       initD3d11().
       initWgcRtCtx().
       startSession();
    return core_ptr->isSuccess();
}

bool FAAWindowCapture::read(frame::FaaMat& mat) {

    WgcTextureData texture_data;
    return this->getFrame(texture_data, mat);

}


