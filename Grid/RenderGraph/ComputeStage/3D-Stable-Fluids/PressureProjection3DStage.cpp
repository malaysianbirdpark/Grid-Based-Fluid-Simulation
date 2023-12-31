#include "pch.h"
#include "PressureProjection3DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

PressureProjection3DStage::PressureProjection3DStage()
	: Compute3DStage{"3D-Pressure Projection", "./CSO/PressureProjection3D_CS.cso", 8, 8, 8}
{
    _uav.resize(1);
    _srv.resize(2);
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

	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());

    _pInID = NodeManager::IssueIncomingAttrID();
    _incoming[_pInID] = -1;
    _attrNames[_pInID] = { "Pressure in" };

    _velocityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_velocityInID] = -1;
    _attrNames[_velocityInID] = { "Velocity In" };

    _velocityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_velocityOutID] = -1;
    _attrNames[_velocityOutID] = { "Velocity Out" };
}

void PressureProjection3DStage::Run(ID3D11DeviceContext& context) {
    Compute3DStage::Run(context);
}

void PressureProjection3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _pInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _velocityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* PressureProjection3DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _velocityOutID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
	return nullptr;
}

