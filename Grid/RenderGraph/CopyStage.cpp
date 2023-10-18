#include "pch.h"
#include "CopyStage.h"

#include "ImGuiRenderer.h"
#include "imnodes.h"

#include "NodeManager.h"

CopyStage::CopyStage()
	: _src{nullptr}, _dest{nullptr}
{
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Src" };
    _outgoing[NodeManager::IssueOutgoingAttrID()] = -1;
    _attrNames[NodeManager::LastOutgoingAttrID()] = { "Dest" };
}

void CopyStage::Run(ID3D11DeviceContext& context)
{
    if ((_dest != nullptr) & (_src != nullptr))
		context.CopyResource(_dest, _src);
}

void CopyStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
	if (attribute_id == _incoming.begin()->first)
		_src = resource;
	else if (attribute_id == _outgoing.begin()->first)
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

    for (auto& in : _incoming) {
		ImNodes::BeginInputAttribute(in.first);
		ImGui::Text("%s (%d)", _attrNames.at(in.first).c_str(), in.first);
		ImNodes::EndInputAttribute();
    }

    for (auto& out : _outgoing) {
        ImNodes::BeginOutputAttribute(out.first);
        ImGui::Text("%s (%d)", _attrNames.at(out.first).c_str(), out.first);
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

FeedBackStage::FeedBackStage(ID3D11Resource* dest)
{
    _name = { "FeedBack" };
    _stageName = { "FeedBackStage" };
    _dest = dest;
}
