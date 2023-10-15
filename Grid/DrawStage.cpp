#include "pch.h"
#include "DrawStage.h"

#include "imgui.h"
#include "imnodes.h"
#include "RenderObject.h"

DrawStage::DrawStage(char const* name, std::shared_ptr<RenderObject> object)
	: _name{name}, _object{object}
{
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
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Parent");
    ImNodes::EndInputAttribute();

    for (auto& child : _outgoing) {
        ImNodes::BeginOutputAttribute(child << 8);
        ImGui::Text("Child %d", child);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}
