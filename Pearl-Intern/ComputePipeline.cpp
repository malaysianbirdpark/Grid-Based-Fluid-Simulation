#include "pch.h"
#include "ComputePipeline.h"

#include <string>
#include <d3dcompiler.h>

ComputePipeline::ComputePipeline(ID3D11Device& device, char const* path, int group_x, int group_y, int group_z)
    : _groupX{group_x}, _groupY{group_y}, _groupZ{group_z}
{
    std::string _path{ path };
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
}

void ComputePipeline::Run(ID3D11DeviceContext& context)
{
    context.CSSetUnorderedAccessViews(0u, _uav.size(), _uav[0].GetAddressOf(), 0u);
    context.CSSetShader(_cs.Get(), nullptr, 0u);
    context.Dispatch(_groupX, _groupY, _groupZ);
    SetBarrier(context);
}

void ComputePipeline::SetBarrier(ID3D11DeviceContext& context)
{
    ID3D11ShaderResourceView* nullSRV[2] = { 0, 0 };
    context.CSSetShaderResources(0, 2, nullSRV);
    ID3D11UnorderedAccessView* nullUAV[2] = { 0, 0 };
    context.CSSetUnorderedAccessViews(0, 2, nullUAV, NULL);
}
