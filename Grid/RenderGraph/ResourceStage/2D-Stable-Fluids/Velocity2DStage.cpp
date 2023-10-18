#include "pch.h"
#include "Velocity2DStage.h"

#include "imnodes.h"

#include "NodeManager.h"

Velocity2DStage::Velocity2DStage()
    : ResourceStage{"2D Velocity Buffer"}
{
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Modifier" };
    _outgoing[NodeManager::IssueOutgoingAttrID()] = -1;
    _attrNames[NodeManager::LastOutgoingAttrID()] = { "Feed to" };

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	pDevice->CreateTexture2D(&desc, nullptr, _resource.ReleaseAndGetAddressOf());
}

Velocity2DStage::~Velocity2DStage()
{
}

ID3D11Resource* Velocity2DStage::Expose(int32_t attribute_id) const
{
	return static_cast<ID3D11Resource*>(_resource.Get());
}
