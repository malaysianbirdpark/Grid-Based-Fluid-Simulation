#include "pch.h"
#include "CopyStage.h"

#include "imnodes.h"

CopyStage::CopyStage()
	: _src{nullptr}, _dest{nullptr}
{
    _incoming.resize(1);
    _outgoing.resize(1);
}

void CopyStage::Run(ID3D11DeviceContext& context)
{
    if ((_dest != nullptr) & (_src != nullptr))
		context.CopyResource(_dest, _src);
}

void CopyStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
	if (attribute_id == 0)
		_src = resource;
	else if (attribute_id == 1)
		_dest = resource;
}

void CopyStage::RenderNode() const
{
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Copy From");
    ImNodes::EndInputAttribute();

    for (auto& child : _outgoing) {
        ImNodes::BeginOutputAttribute(child << 4);
        ImGui::Text("Copy To");
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

ResolveStage::ResolveStage()
{
    _name = { "ResolveStage" };
    _stageName = { "ResolveStage" };
}

void ResolveStage::Run(ID3D11DeviceContext& context)
{
    if ((_dest != nullptr) & (_src != nullptr))
		context.ResolveSubresource(_dest, 0u, _src, 0u, DXGI_FORMAT_R8G8B8A8_UNORM);
}
