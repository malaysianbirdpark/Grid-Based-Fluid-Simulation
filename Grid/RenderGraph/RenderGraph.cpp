#include "pch.h"
#include "RenderGraph.h"

#include <queue>
#include <utility>

#include "imgui.h"
#include "imnodes.h"

#include "ImGuiRenderer.h"

RenderGraph::RenderGraph()
{
}

void RenderGraph::Run(ID3D11DeviceContext& context)
{
    for (auto const node : _orderOfExecution)
        std::visit(Stage::Run{context}, _graph[node]);

    ImGuiShowRenderGraphEditWindow();
}

void RenderGraph::AddStage(Stage::Stage stage)
{
    _graph.emplace_back(std::move(stage));

	auto const id = ImGuiNodeManager::IssueNodeID();
	_indegree[id] = 0;

    std::visit(Stage::SetID{ id }, _graph.back());

    _links.emplace_back();
}

void RenderGraph::Link(int32_t from, int32_t from_attr, int32_t to, int32_t to_attr)
{
	std::visit(Stage::AddOutgoing{to, from_attr}, _graph[from]);
	std::visit(Stage::AddIncoming{from, to_attr}, _graph[to]);

    std::string const from_type{ std::visit(Stage::GetStageName{}, _graph[from]) };
    std::string const to_type{ std::visit(Stage::GetStageName{}, _graph[to]) };

    if (from_type == "Resource")
		std::visit(Stage::Consume{std::visit(Stage::Expose{from_attr}, _graph[from]), to_attr}, _graph[to]);
    else if (to_type == "Resource")
		std::visit(Stage::Consume{ std::visit(Stage::Expose{to_attr}, _graph[to]), from_attr }, _graph[from]);

	++_indegree[to];

	_links[from].emplace_back(to);

    RecalculateTopology();
}

void RenderGraph::ImGuiShowRenderGraphEditWindow()
{
    if (ImGui::Begin("Render Graph")) {
		ImNodes::BeginNodeEditor();

        std::visit(Stage::RenderNode{}, _graph[0]);

        for (auto node_id{ 1 }; node_id < _graph.size(); ++node_id)
            std::visit(Stage::RenderNode{}, _graph[node_id]);

        auto id{ 0 };
        for (auto const& v : _links) {
            for (auto const to : v) 
                ImNodes::Link(id++, to << 8, to);
        }

		ImNodes::EndNodeEditor();
    }
	ImGui::End();
}

void RenderGraph::RecalculateTopology() {
    _orderOfExecution.clear();

    auto indeg {_indegree};

    auto q {std::queue<int32_t> {}};
    for (auto const& it : indeg)
        if (it.second == 0)
            q.emplace(it.first);

    while (!q.empty()) {
        auto const cur {q.front()};
        q.pop();

        _orderOfExecution.emplace_back(cur);

        for (auto const i : std::visit(Stage::GetOutgoings{}, _graph[cur])) {
            --indeg[i];
            if (indeg[i] == 0)
                q.emplace(i);
        }
    }
}
