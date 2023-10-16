#include "pch.h"
#include "Advection2DStage.h"

#include "ImGuiRenderer.h"
#include "imnodes.h"

Advection2DStage::Advection2DStage()
    : ComputeStage("2D-Fluid Advection Stage", "./CSO/Advection2D_CS.cso", 32, 32, 1)
{
    _uav.resize(2);
    _srv.resize(2);
    _nullUav.resize(2);
    _nullSrv.resize(2);

    _incoming.emplace_back(ImGuiNodeManager::IssueAttrID());
    _incoming.emplace_back(ImGuiNodeManager::IssueAttrID());
    _outgoing.emplace_back(ImGuiNodeManager::IssueAttrID());
    _outgoing.emplace_back(ImGuiNodeManager::IssueAttrID());
}

Advection2DStage::~Advection2DStage()
{
}

void Advection2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id > 3) return;

    if (attribute_id == 0) {
        auto desc{ CD3D11_SHADER_RESOURCE_VIEW_DESC{} };
        desc.Format = DXGI_FORMAT_R16G16_FLOAT;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        pDevice->CreateShaderResourceView(resource, &desc, _srv[0].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == 1) {
        auto desc{ CD3D11_SHADER_RESOURCE_VIEW_DESC{} };
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        pDevice->CreateShaderResourceView(resource, &desc, _srv[1].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == 2) {
		auto desc{ CD3D11_UNORDERED_ACCESS_VIEW_DESC{} };
        desc.Format = DXGI_FORMAT_R16G16_FLOAT;
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

        pDevice->CreateUnorderedAccessView(resource, &desc, _uav[0].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == 3) {
		auto desc{ CD3D11_UNORDERED_ACCESS_VIEW_DESC{} };
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

        pDevice->CreateUnorderedAccessView(resource, &desc, _uav[1].ReleaseAndGetAddressOf());
	}
}

ID3D11Resource* Advection2DStage::Expose(int32_t attribute_id)
{
	return nullptr;
}

void Advection2DStage::RenderNode() const
{
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_incoming[0]);
    ImGui::Text("Velocity Input");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(_incoming[1]);
    ImGui::Text("Quantity Input");
    ImNodes::EndInputAttribute();

	ImNodes::BeginOutputAttribute(_outgoing[0] << 8);
	ImGui::Text("Velocity Output");
	ImNodes::EndOutputAttribute();

	ImNodes::BeginOutputAttribute(_outgoing[1] << 8);
	ImGui::Text("Quantity Output");
	ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}
