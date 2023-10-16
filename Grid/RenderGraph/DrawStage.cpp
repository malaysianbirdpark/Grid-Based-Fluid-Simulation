#include "pch.h"
#include "DrawStage.h"

#include "imgui.h"
#include "imnodes.h"
#include "RenderObject.h"

DrawStage::DrawStage(char const* name, std::shared_ptr<RenderObject> object)
	: _name{name}, _object{object}
{
    _incoming.resize(1);
    _outgoing.resize(1);
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

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Incoming");
    ImNodes::EndInputAttribute();

    for (auto& child : _outgoing) {
        ImNodes::BeginOutputAttribute(child << 4);
        ImGui::Text("Outgoing");
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}
