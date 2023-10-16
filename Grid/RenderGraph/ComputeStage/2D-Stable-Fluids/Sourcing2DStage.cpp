#include "pch.h"
#include "Sourcing2DStage.h"

#include "ImGuiRenderer.h"
#include "imnodes.h"

Sourcing2DStage::Sourcing2DStage()
    : ComputeStage{"2D-Fluid Sourcing Stage", "./CSO/Sourcing2D_CS.cso", 32, 32, 1}
{
    _uav.resize(2);
    _srv.resize(1);
    _nullUav.resize(2);
    _nullSrv.resize(1);
    _incoming.emplace_back(ImGuiNodeManager::IssueAttrID());
    _outgoing.emplace_back(ImGuiNodeManager::IssueAttrID());
    _outgoing.emplace_back(ImGuiNodeManager::IssueAttrID());
}

Sourcing2DStage::~Sourcing2DStage()
{
}

void Sourcing2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id > 1) return;

    if (attribute_id == 0) {
		auto desc {CD3D11_UNORDERED_ACCESS_VIEW_DESC{}};
		desc.Format = DXGI_FORMAT_R16G16_FLOAT;
		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		pDevice->CreateUnorderedAccessView(resource, &desc, _uav[attribute_id].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == 1) {
		auto desc {CD3D11_UNORDERED_ACCESS_VIEW_DESC{}};
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		pDevice->CreateUnorderedAccessView(resource, &desc, _uav[attribute_id].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* Sourcing2DStage::Expose(int32_t attribute_id)
{
    ID3D11Resource* resource {nullptr};
    if (_uav[attribute_id])
		_uav[attribute_id]->GetResource(&resource);
    return resource;
}

void Sourcing2DStage::RenderNode() const
{
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Incoming");
    ImNodes::EndInputAttribute();

	ImNodes::BeginOutputAttribute(_outgoing[0] << 8);
	ImGui::Text("Velocity");
	ImNodes::EndOutputAttribute();

	ImNodes::BeginOutputAttribute(_outgoing[1] << 8);
	ImGui::Text("Quantity");
	ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}
