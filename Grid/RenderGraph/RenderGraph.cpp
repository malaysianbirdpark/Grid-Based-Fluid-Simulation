#include "pch.h"
#include "RenderGraph.h"

#include <queue>
#include <utility>
#include <iostream>

#include "imgui.h"
#include "imnodes.h"

#include "NodeManager.h"

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

	auto const id = NodeManager::IssueNodeID();
	_indegree[id] = 0;
    _topology[id] = {};

    std::visit(Stage::SetID{ id }, _graph.back());
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
    _topology[from].emplace_back(to);

    _links.emplace_back(from_attr, to_attr);

    RecalculateTopology();
}

void RenderGraph::ImGuiShowRenderGraphEditWindow()
{
    if (ImGui::Begin("Render Graph")) {
		ImNodes::BeginNodeEditor();

        for (auto node_id{ 0 }; node_id < _graph.size(); ++node_id)
            std::visit(Stage::RenderNode{}, _graph[node_id]);

        auto id{ 0 };
        for (auto const& [start, end] : _links) {
			ImNodes::Link(id++, start, end);
        }

		ImNodes::EndNodeEditor();
    }
	ImGui::End();
}

void RenderGraph::Update()
{
    int32_t start{};
    int32_t end{};
    if (ImNodes::IsLinkCreated(&start, &end)) {
		auto const from{ NodeManager::GetOwner(start) };
		auto const to{ NodeManager::GetOwner(end) };
        if ((from != -1) & (to != -1))
			Link(from, start, to, end);
    }
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

        for (auto const i : _topology[cur]) {
			--indeg[i];
			if (indeg[i] == 0)
				q.emplace(i);
        }
    }
}
