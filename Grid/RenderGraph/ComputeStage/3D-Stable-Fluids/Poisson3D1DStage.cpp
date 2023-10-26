#include "pch.h"
#include "Poisson3D1DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

Poisson3D1DStage::Poisson3D1DStage()
    : Compute3DStage{"3D1D-Poisson Solver", "./CSO/Poisson3D1D_CS.cso", 32, 32, 1}
{
    _uav.resize(2);
    _srv.resize(3);
    _nullUav.resize(5);
    _nullSrv.resize(5);

    _resource.resize(1);

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
	desc.MipLevels = 1u;

    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[1].ReleaseAndGetAddressOf());

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x" };

    _bID = NodeManager::IssueIncomingAttrID();
    _incoming[_bID] = -1;
    _attrNames[_bID] = { "b" };

    _constID = NodeManager::IssueIncomingAttrID();
    _incoming[_constID] = -1;
    _attrNames[_constID] = { "Constants(CB)" };

    _xOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_xOutID] = -1;
    _attrNames[_xOutID] = { "new x" };
}

void Poisson3D1DStage::Run(ID3D11DeviceContext& context)
{
    for (auto i {0}; i != 31; ++i) {
        context.OMSetRenderTargets(0u, nullptr, nullptr);
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
}

void Poisson3D1DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID) {
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == _bID) {
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[2].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* Poisson3D1DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _xOutID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
	return nullptr;
}

