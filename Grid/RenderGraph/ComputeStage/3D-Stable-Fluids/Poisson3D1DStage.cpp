#include "pch.h"
#include "Poisson3D1DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

Poisson3D1DStage::Poisson3D1DStage()
    : Compute3DStage{"3D1D-Poisson", "./CSO/Poisson3D1D_CS.cso", 8, 8, 8}
{
    _uav.resize(2);
    _srv.resize(3);
    _nullUav.resize(5);
    _nullSrv.resize(5);

    _resource.resize(2);

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
	desc.MipLevels = 1u;

    desc.Format = DXGI_FORMAT_R16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[0].ReleaseAndGetAddressOf());

    desc.Format = DXGI_FORMAT_R16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[1].Get(), nullptr, _srv[1].ReleaseAndGetAddressOf());

    _divID = NodeManager::IssueIncomingAttrID();
    _incoming[_divID] = -1;
    _attrNames[_divID] = { "Div" };

    _pressureOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_pressureOutID] = -1;
    _attrNames[_pressureOutID] = { "Pressure" };

    {
		std::string _path{"./CSO/Initializer3D_CS.cso"};
		std::wstring p(_path.length(), L' ');
		std::ranges::copy(_path, p.begin());

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(
			p.c_str(),
			pBlob.ReleaseAndGetAddressOf()
		);

		pDevice->CreateComputeShader(
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(),
			nullptr,
			_initCS.ReleaseAndGetAddressOf()
		);
    }

    {
		std::string _path{"./CSO/BoundaryCondition3D_CS.cso"};
		std::wstring p(_path.length(), L' ');
		std::ranges::copy(_path, p.begin());

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(
			p.c_str(),
			pBlob.ReleaseAndGetAddressOf()
		);

		pDevice->CreateComputeShader(
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(),
			nullptr,
			_bcCS.ReleaseAndGetAddressOf()
		);
    }
}

void Poisson3D1DStage::Run(ID3D11DeviceContext& context)
{
    context.OMSetRenderTargets(0u, nullptr, nullptr);

	context.CSSetUnorderedAccessViews(0u, 2u, _uav[0].GetAddressOf(), nullptr);
	context.CSSetShader(_initCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

    for (auto i {0}; i != 15; ++i) {
        context.CSSetShaderResources(0u, 1u, _srv[i & 0b1].GetAddressOf());
        context.CSSetShaderResources(1u, 1u, _srv[2].GetAddressOf());
        context.CSSetUnorderedAccessViews(0u, 1u, _uav[!(i & 0b1)].GetAddressOf(), nullptr);
        context.CSSetShader(_cs.Get(), nullptr, 0u);
        context.Dispatch(
            static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
            static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
            static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
        );
        SetBarrier(context);
    }

    context.CSSetUnorderedAccessViews(0u, 1u, _uav[1].GetAddressOf(), nullptr);
    context.CSSetShader(_bcCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
    );
    SetBarrier(context);
}

void Poisson3D1DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _divID) {
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[2].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* Poisson3D1DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _pressureOutID)
        return static_cast<ID3D11Resource*>(_resource[1].Get());
	return nullptr;
}

