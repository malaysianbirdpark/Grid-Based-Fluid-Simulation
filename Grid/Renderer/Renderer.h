#pragma once

#include <memory>

class Renderer {
public:  static void   Init(int width, int height, HWND native_wnd);
public:  static void   BeginFrame();
public:  static void   EndFrame();
public:  static DirectX::XMMATRIX GetProj();
public:  static ID3D11Device& Device();
public:  static ID3D11DeviceContext& Context();
public:  static IDXGISwapChain& SwapChain();

private: static void   InitRS();
private: static void   InitDS();
private: static void   InitBS();
private: static void   InitSamplers();

private:
	inline static Microsoft::WRL::ComPtr<ID3D11Device>              _device;
	inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext>       _immContext;
	inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext>       _defaultContext;
	inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext>       _imguiContext;
	inline static Microsoft::WRL::ComPtr<IDXGISwapChain>            _swapChain;

    inline static DirectX::XMFLOAT4X4                               _proj;

    inline static Microsoft::WRL::ComPtr<ID3D11Resource>            _backBuffers;
    inline static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    _backBufferView;

    inline static Microsoft::WRL::ComPtr<ID3D11Texture2D>           _viewportBuffer;

    inline static Microsoft::WRL::ComPtr<ID3D11Texture2D>           _imguiBuffer;
    inline static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    _imguiRTV;

    inline static Microsoft::WRL::ComPtr<ID3D11RasterizerState>     _rasterizerState;

	inline static Microsoft::WRL::ComPtr<ID3D11DepthStencilState>   _imguiDS;
	inline static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    _imguiDSV;

    inline static Microsoft::WRL::ComPtr<ID3D11BlendState>          _bsDefault;

    inline static D3D11_VIEWPORT                                    _viewport{};

    inline static Microsoft::WRL::ComPtr<ID3D11SamplerState>        _samplerLinear;
    inline static Microsoft::WRL::ComPtr<ID3D11SamplerState>        _samplerPoint;
    inline static Microsoft::WRL::ComPtr<ID3D11SamplerState>        _yeah;
};
 