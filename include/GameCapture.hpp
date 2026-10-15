#pragma once
#include <cstdint>
#include <vector>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct FrameView {
    const uint8_t* data = nullptr;
    int width = 0;
    int height = 0;
    int stride = 0;
    bool valid = false;
};

class GameCapture {
public:
    GameCapture(int fovW, int fovH, int offX, int offY);
    ~GameCapture();
    GameCapture(const GameCapture&) = delete;
    GameCapture& operator=(const GameCapture&) = delete;
    bool ok() const { return initialized_; }
    FrameView get_frame();
private:
    bool initD3D();
    bool initDuplication();
    bool reinit();
    void releaseAll();
    int fovW_, fovH_, offX_, offY_;
    int screenW_ = 0, screenH_ = 0;
    bool initialized_ = false;
    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> ctx_;
    ComPtr<IDXGIOutputDuplication> dup_;
    ComPtr<ID3D11Texture2D> staging_;
    std::vector<uint8_t> fovBuf_;
    int fovStride_ = 0;
    FrameView lastView_{};
};
