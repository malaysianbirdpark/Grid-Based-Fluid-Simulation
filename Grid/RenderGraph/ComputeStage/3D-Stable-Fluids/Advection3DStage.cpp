#include "pch.h"
#include "Advection3DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

Advection3DStage::Advection3DStage()
    : Compute3DStage{ "3D-Advection", "./CSO/Advection3D_CS.cso", 8, 8, 8 }
{
    _uav.resize(2);
    _srv.resize(4);
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

	desc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[2].ReleaseAndGetAddressOf());

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[1].Get(), nullptr, _srv[3].ReleaseAndGetAddressOf());

    _cbTimestepID = NodeManager::IssueIncomingAttrID();
    _incoming[_cbTimestepID] = -1;
    _attrNames[_cbTimestepID] = { "Timestep(CB)" };

    _velocityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_velocityInID] = -1;
    _attrNames[_velocityInID] = { "Velocity in" };

    _quantityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_quantityInID] = -1;
    _attrNames[_quantityInID] = { "Quantity in" };

    _velocityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_velocityOutID] = -1;
    _attrNames[_velocityOutID] = { "Velocity out" };

    _quantityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_quantityOutID] = -1;
    _attrNames[_quantityOutID] = { "Quantity out" };
}

void Advection3DStage::Run(ID3D11DeviceContext& context)
{
    context.OMSetRenderTargets(0u, nullptr, nullptr);
    context.CSSetShaderResources(0u, 2u, _srv[0].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 2u, _uav[0].GetAddressOf(), nullptr);
    context.CSSetShader(_cs.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);
}

void Advection3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _quantityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* Advection3DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _velocityOutID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
    else if (attribute_id == _quantityOutID)
        return static_cast<ID3D11Resource*>(_resource[1].Get());
    return nullptr;
}