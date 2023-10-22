#include "pch.h"
#include "Sourcing2DStage.h"

#include "NodeManager.h"

Sourcing2DStage::Sourcing2DStage()
    : ComputeStage{"2D-Fluid Sourcing", "./CSO/Sourcing2D_CS.cso", 32, 32, 1}
{
    _uav.resize(2);
    _srv.resize(2);
    _nullUav.resize(2);
    _nullSrv.resize(2);

    _resource.resize(2);

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	//desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

    pDevice->CreateTexture2D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());

	//desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture2D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());

    _fluidColorID = NodeManager::IssueIncomingAttrID();
    _incoming[_fluidColorID] = -1;
    _attrNames[_fluidColorID] = { "Fluid Color(CB)" };

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

void Sourcing2DStage::Run(ID3D11DeviceContext& context) {
    ComputeStage::Run(context);
}

void Sourcing2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (resource) {
        if (attribute_id == _velocityInID)
            pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
        else if (attribute_id == _quantityInID)
            pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* Sourcing2DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _velocityOutID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
    else if (attribute_id == _quantityOutID)
        return static_cast<ID3D11Resource*>(_resource[1].Get());
    return nullptr;
}