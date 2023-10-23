#include "pch.h"
#include "Renderer.h"

#include <array>
#include <string>
#include <d3dcompiler.h>

#include "ImGuiRenderer.h"


void Renderer::Init(int width, int height, HWND native_wnd)
{
	DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 3;
    sd.OutputWindow = native_wnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.Flags = 0u;

    auto swap_chain_flag{ 0u };
#ifdef _DEBUG
    swap_chain_flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level[]{ D3D_FEATURE_LEVEL_11_1 };
    ::D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swap_chain_flag,
        feature_level,
        1,
        D3D11_SDK_VERSION,
        &sd,
        _swapChain.ReleaseAndGetAddressOf(),
        _device.ReleaseAndGetAddressOf(),
        nullptr,
        _immContext.ReleaseAndGetAddressOf()
    );

    auto& device{ *_device.Get() };
    auto& context{ *_immContext.Get() };
    auto& swap_chain{ *_swapChain.Get() };

    DirectX::XMStoreFloat4x4(&_proj, DirectX::XMMatrixPerspectiveFovLH(
        70.0f,
        static_cast<float>(gViewportInfo.width) / gViewportInfo.height,
        0.01f,
        200.0f
    ));

    swap_chain.GetBuffer(0u, IID_PPV_ARGS(_backBuffers.ReleaseAndGetAddressOf()));
    device.CreateRenderTargetView(_backBuffers.Get(), nullptr, _backBufferView.ReleaseAndGetAddressOf());

    D3D11_TEXTURE2D_DESC desc;
    static_cast<ID3D11Texture2D*>(_backBuffers.Get())->GetDesc(&desc);
    desc.MipLevels = desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    device.CreateTexture2D(&desc, nullptr, _imguiBuffer.ReleaseAndGetAddressOf());
    device.CreateRenderTargetView(_imguiBuffer.Get(), nullptr, _imguiRTV.ReleaseAndGetAddressOf());

    desc.Width = gViewportInfo.width;
    desc.Height = gViewportInfo.height;
    device.CreateTexture2D(&desc, nullptr, _viewportBuffer.ReleaseAndGetAddressOf());
    device.CreateShaderResourceView(_viewportBuffer.Get(), nullptr, pViewportSRV.ReleaseAndGetAddressOf());

    _viewport.Width = static_cast<float>(width);
    _viewport.Height = static_cast<float>(height);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;
    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;

    device.CreateDeferredContext(0u, _defaultContext.ReleaseAndGetAddressOf());
    device.CreateDeferredContext(0u, _imguiContext.ReleaseAndGetAddressOf());

	ImGuiRenderer::Init(_imguiContext.Get());

    pDevice = _device.Get();

    InitRS();
    InitDS(width, height);
    InitBS();
    InitSamplers();
}

void Renderer::BeginFrame()
{
	static float constexpr clear_color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

    _defaultContext->RSSetViewports(1u, &_viewport);
    _defaultContext->RSSetState(_rasterizerState.Get());
    _defaultContext->OMSetDepthStencilState(_dsDefault.Get(), 0u);
    _defaultContext->OMSetBlendState(_bsDefault.Get(), nullptr, 0xFFFFFFFFu);

    _defaultContext->OMSetRenderTargets(1u, _backBufferView.GetAddressOf(), _dsv.Get());
    _defaultContext->ClearRenderTargetView(_backBufferView.Get(), clear_color);
    _defaultContext->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

    _defaultContext->PSSetSamplers(0u, 1u, _samplerLinear.GetAddressOf());
    _defaultContext->CSSetSamplers(0u, 1u, _samplerLinear.GetAddressOf());
    _defaultContext->CSSetSamplers(1u, 1u, _samplerPoint.GetAddressOf());

	_imguiContext->RSSetViewports(1u, &_viewport);
    _imguiContext->RSSetState(_rasterizerState.Get());
    _imguiContext->OMSetDepthStencilState(_dsDefault.Get(), 0u);
    _imguiContext->OMSetRenderTargets(1u, _imguiRTV.GetAddressOf(), nullptr);

    ImGuiRenderer::BeginFrame();
}

void Renderer::EndFrame()
{
    ImGuiRenderer::EndFrame();

    _imguiContext->CopyResource(_backBuffers.Get(), _imguiBuffer.Get());

    static std::array<ID3D11CommandList*, 2> cmd_lists{};
    _defaultContext->FinishCommandList(FALSE, &cmd_lists[0]);
    _imguiContext->FinishCommandList(FALSE, &cmd_lists[1]);

	_immContext->ExecuteCommandList(cmd_lists[0], FALSE);
	_immContext->ExecuteCommandList(cmd_lists[1], FALSE);

    cmd_lists[0]->Release();
    cmd_lists[1]->Release();

    _swapChain->Present(1u, 0u);
}

DirectX::XMMATRIX Renderer::GetProj()
{
    return DirectX::XMLoadFloat4x4(&_proj);
}

ID3D11Device& Renderer::Device()
{
    return *_device.Get();
}

ID3D11DeviceContext& Renderer::Context()
{
    return *_defaultContext.Get();
}

IDXGISwapChain& Renderer::SwapChain()
{
    return *_swapChain.Get();
}

void Renderer::InitRS()
{
    D3D11_RASTERIZER_DESC rd{};

    // Default Solid
    {
        rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
        rd.FrontCounterClockwise = false;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = true;

        Device().CreateRasterizerState(&rd, _rasterizerState.ReleaseAndGetAddressOf());
    }
}

void Renderer::InitDS(int width, int height)
{
    // default
    {
        D3D11_DEPTH_STENCIL_DESC desc{ CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}} };

        Device().CreateDepthStencilState(&desc, _dsDefault.ReleaseAndGetAddressOf());
    }

    D3D11_TEXTURE2D_DESC descDepth{};
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _ds;
    Device().CreateTexture2D(&descDepth, nullptr, _ds.ReleaseAndGetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv{};
    desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc_dsv.Texture2D.MipSlice = 0u;
    Device().CreateDepthStencilView(_ds.Get(), &desc_dsv, _dsv.ReleaseAndGetAddressOf());
}

void Renderer::InitBS()
{
    // Default
    {
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = true;
        desc.IndependentBlendEnable = false;

        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        Device().CreateBlendState(&desc, _bsDefault.ReleaseAndGetAddressOf());
    }
}

void Renderer::InitSamplers()
{
    {
        D3D11_SAMPLER_DESC sd{};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.BorderColor[0] = 1.0f;
        sd.BorderColor[1] = 0.0f;
        sd.BorderColor[2] = 0.0f;
        sd.BorderColor[3] = 1.0f;
        sd.MaxAnisotropy = 1;
        sd.MipLODBias = 0.0f;
        sd.MinLOD = 0.0f;
        //sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.MaxLOD = 0.0f;

        pDevice->CreateSamplerState(&sd, _samplerLinear.ReleaseAndGetAddressOf());
    }

    {
        D3D11_SAMPLER_DESC sd{};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.BorderColor[0] = 0.0f;
        sd.BorderColor[1] = 0.0f;
        sd.BorderColor[2] = 0.0f;
        sd.BorderColor[3] = 1.0f;
        sd.MaxAnisotropy = 1;
        sd.MipLODBias = 0.0f;
        sd.MinLOD = 0.0f;
        //sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.MaxLOD = 0.0f;

        pDevice->CreateSamplerState(&sd, _samplerPoint.ReleaseAndGetAddressOf());
    }
}

