#include "pch.h"
#include "BackBufferStage.h"

#include "ImGuiRenderer.h"
#include "imnodes.h"

BackBufferStage::BackBufferStage(IDXGISwapChain& swap_chain)
    : ResourceStage{"Back-Buffer"}
{
    swap_chain.GetBuffer(0u, IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf()));
    _incoming.emplace_back(ImGuiNodeManager::IssueAttrID());
    _outgoing.emplace_back(ImGuiNodeManager::IssueAttrID());
}

BackBufferStage::~BackBufferStage() {
}

ID3D11Resource* BackBufferStage::Expose(int32_t attribute_id) const {
    return _resource.Get();
}

void BackBufferStage::RenderNode() const {
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Incoming");
    ImNodes::EndInputAttribute();

    for (auto& child : _outgoing) {
        ImNodes::BeginOutputAttribute(child << 8);
        ImGui::Text("Outgoing %d", child);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}
