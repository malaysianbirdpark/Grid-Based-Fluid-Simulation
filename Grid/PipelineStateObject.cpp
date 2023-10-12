#include "pch.h"
#include "PipelineStateObject.h"

#include <string>
#include <d3dcompiler.h>

void PipelineStateObject::SetInputLayout(ID3D11Device& device, std::vector<D3D11_INPUT_ELEMENT_DESC> const& layout)
{
    device.CreateInputLayout(
        layout.data(),
        static_cast<UINT>(layout.size()),    
        _vsByteCode->GetBufferPointer(),
        _vsByteCode->GetBufferSize(),
        _inputLayout.ReleaseAndGetAddressOf()
    );
}

void PipelineStateObject::SetVertexShader(ID3D11Device& device, char const* path)
{
    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    D3DReadFileToBlob(
        p.c_str(),
        _vsByteCode.ReleaseAndGetAddressOf()
    );

    device.CreateVertexShader(
        _vsByteCode->GetBufferPointer(),
        _vsByteCode->GetBufferSize(),
        nullptr,
        _vs.ReleaseAndGetAddressOf()
    );
}

void PipelineStateObject::SetGeometryShader(ID3D11Device& device, char const* path)
{
    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    D3DReadFileToBlob(
        p.c_str(),
        pBlob.ReleaseAndGetAddressOf()
    );

    device.CreateGeometryShader(
        pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(),
        nullptr,
        _gs.ReleaseAndGetAddressOf()
    );
}

void PipelineStateObject::SetPixelShader(ID3D11Device& device, char const* path)
{
    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    D3DReadFileToBlob(p.c_str(), pBlob.ReleaseAndGetAddressOf());
    device.CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, _ps.ReleaseAndGetAddressOf());
}

void PipelineStateObject::Bind(ID3D11DeviceContext& context)
{
    context.IASetInputLayout(_inputLayout.Get());
    context.VSSetShader(_vs.Get(), nullptr, 0u);
    context.GSSetShader(_gs.Get(), nullptr, 0u);
    context.PSSetShader(_ps.Get(), nullptr, 0u);
}
