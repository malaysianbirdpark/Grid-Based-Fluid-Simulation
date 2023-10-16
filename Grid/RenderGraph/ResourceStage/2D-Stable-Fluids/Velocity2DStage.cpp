#include "pch.h"
#include "Velocity2DStage.h"

#include "ImGuiRenderer.h"
#include "imnodes.h"

Velocity2DStage::Velocity2DStage()
    : ResourceStage{"2D Velocity Buffer"}
{
    _incoming.emplace_back(ImGuiNodeManager::IssueAttrID());
    _outgoing.emplace_back(ImGuiNodeManager::IssueAttrID());

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Width = 800;
	desc.Height = 600;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	pDevice->CreateTexture2D(&desc, nullptr, _resource.ReleaseAndGetAddressOf());
}

Velocity2DStage::~Velocity2DStage()
{
}

ID3D11Resource* Velocity2DStage::Expose(int32_t attribute_id) const
{
	return _resource.Get();
}

void Velocity2DStage::RenderNode() const
{
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Modifier");
    ImNodes::EndInputAttribute();

    for (auto& child : _outgoing) {
        ImNodes::BeginOutputAttribute(child << 8);
        ImGui::Text("Feed to", child);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();

}
