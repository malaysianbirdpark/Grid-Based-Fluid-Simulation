#include "pch.h"
#include "Pressure3D1DStage.h"

#include "NodeManager.h"

Pressure3D1DStage::Pressure3D1DStage()
	: ResourceStage{"3D1D-Pressure"}
{
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Modifier" };
    _outgoing[NodeManager::IssueOutgoingAttrID()] = -1;
    _attrNames[NodeManager::LastOutgoingAttrID()] = { "Feed to" };

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
	desc.MipLevels = 1u;

	pDevice->CreateTexture3D(&desc, nullptr, _resource.ReleaseAndGetAddressOf());
}

ID3D11Resource* Pressure3D1DStage::Expose(int32_t attribute_id) const
{
	return static_cast<ID3D11Resource*>(_resource.Get());
}

