#include "pch.h"
#include "PipelineStateObject.h"

#include <string>
#include <d3dcompiler.h>

void PipelineStateObject::SetInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> const& layout)
{
    pDevice->CreateInputLayout(
        layout.data(),
        static_cast<UINT>(layout.size()),    
        _vsByteCode->GetBufferPointer(),
        _vsByteCode->GetBufferSize(),
        _inputLayout.ReleaseAndGetAddressOf()
    );
}

void PipelineStateObject::SetVertexShader(char const* path)
{
    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    D3DReadFileToBlob(
        p.c_str(),
        _vsByteCode.ReleaseAndGetAddressOf()
    );

    pDevice->CreateVertexShader(
        _vsByteCode->GetBufferPointer(),
        _vsByteCode->GetBufferSize(),
        nullptr,
        _vs.ReleaseAndGetAddressOf()
    );
}

void PipelineStateObject::SetGeometryShader(char const* path)
{
    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    D3DReadFileToBlob(
        p.c_str(),
        pBlob.ReleaseAndGetAddressOf()
    );

    pDevice->CreateGeometryShader(
        pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(),
        nullptr,
        _gs.ReleaseAndGetAddressOf()
    );
}

void PipelineStateObject::SetPixelShader(char const* path)
{
    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    D3DReadFileToBlob(p.c_str(), pBlob.ReleaseAndGetAddressOf());
    pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, _ps.ReleaseAndGetAddressOf());
}

void PipelineStateObject::Bind(ID3D11DeviceContext& context)
{
    context.IASetInputLayout(_inputLayout.Get());
    context.VSSetShader(_vs.Get(), nullptr, 0u);
    context.GSSetShader(_gs.Get(), nullptr, 0u);
    context.PSSetShader(_ps.Get(), nullptr, 0u);
}
