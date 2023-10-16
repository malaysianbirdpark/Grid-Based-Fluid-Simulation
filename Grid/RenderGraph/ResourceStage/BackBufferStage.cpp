#include "pch.h"
#include "BackBufferStage.h"

#include "imgui.h"
#include "imnodes.h"

BackBufferStage::BackBufferStage(IDXGISwapChain& swap_chain)
    : ResourceStage{"Back-Buffer"}
{
    swap_chain.GetBuffer(0u, IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf()));
    _incoming.resize(1);
    _outgoing.resize(1);
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
