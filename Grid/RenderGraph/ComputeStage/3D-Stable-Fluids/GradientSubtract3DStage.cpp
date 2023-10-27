#include "pch.h"
#include "GradientSubtract3DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

GradientSubtract3DStage::GradientSubtract3DStage()
	: Compute3DStage{"3D-GradientSubtract", "./CSO/GradientSub3D_CS.cso", 8, 8, 8}
{
    _uav.resize(3);
    _srv.resize(1);
    _nullUav.resize(5);
    _nullSrv.resize(5);

    _resource.resize(1);

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
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[2].ReleaseAndGetAddressOf());

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x in" };

    _subTargetID = NodeManager::IssueIncomingAttrID();
    _incoming[_subTargetID] = -1;
    _attrNames[_subTargetID] = { "Subtract target" };

    _resultID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_resultID] = -1;
    _attrNames[_resultID] = { "Result" };
}

void GradientSubtract3DStage::Run(ID3D11DeviceContext& context) {
    Compute3DStage::Run(context);
}

void GradientSubtract3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _subTargetID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* GradientSubtract3DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _resultID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
	return nullptr;
}

