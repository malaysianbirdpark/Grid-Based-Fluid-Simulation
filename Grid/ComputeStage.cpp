#include "pch.h"
#include "ComputeStage.h"

#include <string>
#include <d3dcompiler.h>

ComputeStage::ComputeStage(ID3D11Device& device, char const* compute_shader_path, std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>&& uav, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>&& srv, int32_t group_x, int32_t group_y, int32_t group_z)
    : _uav{std::move(uav)}, _srv{std::move(srv)}, _groupX{group_x}, _groupY{group_y}, _groupZ{group_z}
{
    std::string _path{ compute_shader_path };
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

void ComputeStage::Run(ID3D11DeviceContext& context) {
	context.CSSetShaderResources(0u, static_cast<UINT>(_srv.size()), _srv[0].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, static_cast<UINT>(_uav.size()), _uav[0].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(_groupX, _groupY, _groupZ);
	SetBarrier(context);
}

void ComputeStage::SetBarrier(ID3D11DeviceContext& context) {
    context.CSSetShaderResources(0, static_cast<UINT>(_nullSrv.size()), &_nullSrv[0]);
    context.CSSetUnorderedAccessViews(0, static_cast<UINT>(_nullUav.size()), &_nullUav[0], nullptr);
}
