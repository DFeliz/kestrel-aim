#include "GameCapture.hpp"
#include <cstring>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

GameCapture::GameCapture(int fovW, int fovH, int offX, int offY)
    : fovW_(fovW), fovH_(fovH), offX_(offX), offY_(offY) {
    initialized_ = initD3D() && initDuplication();
}

GameCapture::~GameCapture(){ releaseAll(); }

void GameCapture::releaseAll(){
    staging_.Reset(); dup_.Reset(); ctx_.Reset(); device_.Reset();
    initialized_ = false;
}

bool GameCapture::initD3D(){
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL got{};
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, levels, _countof(levels), D3D11_SDK_VERSION, &device_, &got, &ctx_);
    return SUCCEEDED(hr);
}

bool GameCapture::initDuplication(){
    ComPtr<IDXGIDevice> dxgiDev;
    if(FAILED(device_.As(&dxgiDev))) return false;
    ComPtr<IDXGIAdapter> adapter;
    if(FAILED(dxgiDev->GetAdapter(&adapter))) return false;
    ComPtr<IDXGIOutput> output;
    if(FAILED(adapter->EnumOutputs(0, &output))) return false;
    DXGI_OUTPUT_DESC od{};
    if(FAILED(output->GetDesc(&od))) return false;
    screenW_ = od.DesktopCoordinates.right - od.DesktopCoordinates.left;
    screenH_ = od.DesktopCoordinates.bottom - od.DesktopCoordinates.top;
    ComPtr<IDXGIOutput1> out1;
    if(FAILED(output.As(&out1))) return false;
    if(FAILED(out1->DuplicateOutput(device_.Get(), &dup_))){ dup_.Reset(); return false; }

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = (UINT)screenW_;
    desc.Height = (UINT)screenH_;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    if(FAILED(device_->CreateTexture2D(&desc, nullptr, &staging_))){ dup_.Reset(); return false; }
    fovStride_ = fovW_ * 4;
    fovBuf_.resize((size_t)fovStride_ * (size_t)fovH_);
    return true;
}

bool GameCapture::reinit(){
    releaseAll();
    initialized_ = initD3D() && initDuplication();
    return initialized_;
}

FrameView GameCapture::get_frame(){
    lastView_.valid = false;
    if(!initialized_ && !reinit()) return lastView_;

    DXGI_OUTDUPL_FRAME_INFO info{};
    ComPtr<IDXGIResource> res;
    HRESULT hr = dup_->AcquireNextFrame(16, &info, &res);
    if(hr == DXGI_ERROR_WAIT_TIMEOUT) return lastView_;
    if(hr == DXGI_ERROR_ACCESS_LOST || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_INVALID_CALL){
        reinit(); return lastView_;
    }
    if(FAILED(hr)) return lastView_;

    ComPtr<ID3D11Texture2D> tex;
    if(FAILED(res.As(&tex))){ dup_->ReleaseFrame(); return lastView_; }
    ctx_->CopyResource(staging_.Get(), tex.Get());
    dup_->ReleaseFrame();

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if(FAILED(ctx_->Map(staging_.Get(), 0, D3D11_MAP_READ, 0, &mapped))) return lastView_;

    int srcX = (screenW_ - fovW_)/2 + offX_;
    int srcY = (screenH_ - fovH_)/2 + offY_;
    if(srcX < 0) srcX = 0;
    if(srcY < 0) srcY = 0;
    if(srcX + fovW_ > screenW_) srcX = screenW_ - fovW_;
    if(srcY + fovH_ > screenH_) srcY = screenH_ - fovH_;

    const uint8_t* srcBase = static_cast<const uint8_t*>(mapped.pData);
    for(int y = 0; y < fovH_; ++y){
        const uint8_t* srow = srcBase + (size_t)(srcY + y)*mapped.RowPitch + (size_t)srcX*4;
        uint8_t* drow = fovBuf_.data() + (size_t)y*fovStride_;
        std::memcpy(drow, srow, (size_t)fovStride_);
    }
    ctx_->Unmap(staging_.Get(), 0);

    lastView_.data = fovBuf_.data();
    lastView_.width = fovW_;
    lastView_.height = fovH_;
    lastView_.stride = fovStride_;
    lastView_.valid = true;
    return lastView_;
}
