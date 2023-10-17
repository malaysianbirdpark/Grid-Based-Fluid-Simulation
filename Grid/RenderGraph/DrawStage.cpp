#include "pch.h"
#include "DrawStage.h"

#include "imgui.h"
#include "imnodes.h"
#include "RenderObject.h"

#include "NodeManager.h"

DrawStage::DrawStage(char const* name, std::shared_ptr<RenderObject> object)
	: _name{name}, _object{object}
{
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Before" };
    _outgoing[NodeManager::IssueOutgoingAttrID()] = -1;
    _attrNames[NodeManager::LastOutgoingAttrID()] = { "After" };
}

void DrawStage::Run(ID3D11DeviceContext& context)
{
	_object->Draw(context);
}

void DrawStage::RenderNode() const {
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
