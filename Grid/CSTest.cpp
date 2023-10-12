#include "pch.h"
#include "CSTest.h"

#include <string>
#include <d3dcompiler.h>

#include <iostream>

CSTest::CSTest(ID3D11Device& device, IDXGISwapChain& swap_chain)
    : swap_chain{swap_chain}
{
    std::string _path{ "testCS.cso" };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    D3DReadFileToBlob(
        p.c_str(),
        pBlob.ReleaseAndGetAddressOf()
    );

    device.CreateComputeShader(
        pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(),
        nullptr,
        _cs.ReleaseAndGetAddressOf()
    );

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer {};
    swap_chain.GetBuffer(0u, IID_PPV_ARGS(backbuffer.ReleaseAndGetAddressOf()));

    auto desc {CD3D11_UNORDERED_ACCESS_VIEW_DESC{}};
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0u;
    device.CreateUnorderedAccessView(backbuffer.Get(), &desc, _uav.ReleaseAndGetAddressOf());
}

void CSTest::Run(ID3D11DeviceContext& context) {
    context.CSSetUnorderedAccessViews(0u, 1u, _uav.GetAddressOf(), nullptr);
    context.CSSetShader(_cs.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(800.0f / 256.0f)),
        600,
        1
    );
    SetBarrier(context);
}

void CSTest::SetBarrier(ID3D11DeviceContext& context) {
    ID3D11ShaderResourceView* nullSRV[2] = { nullptr };
    context.CSSetShaderResources(0, 2, nullSRV);
    ID3D11UnorderedAccessView* nullUAV[2] = { nullptr };
    context.CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
}
