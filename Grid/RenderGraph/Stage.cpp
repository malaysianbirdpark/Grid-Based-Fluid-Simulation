#include "pch.h"
#include "Stage.h"

#include "imgui.h"
#include "imnodes.h"

void Stage::RootDummyStage::RenderNode() const {
    ImNodes::BeginNode(0);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImNodes::EndNodeTitleBar();

    {
        auto child_id{ 0 };
        for (auto& child : _outgoing) {
            child_id = child;
            ImNodes::BeginOutputAttribute(child_id << 8);
            ImGui::Text("Child %d", child_id);
            ImNodes::EndOutputAttribute();
        }
    }

    ImNodes::EndNode();
}
