#include "pch.h"
#include "RenderGraph.h"

#include <queue>
#include <utility>

#include "imgui.h"
#include "imnodes.h"

RenderGraph::RenderGraph()
{
    _orderOfExecution.emplace_back(0);
    _indegree[0] = 0;
    _links.resize(1);
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

	auto const id =  static_cast<int32_t>(_graph.size() - 1);
	_indegree[id] = 0;

    std::visit(Stage::SetID{ id }, _graph.back());

    _links.emplace_back();
}

void RenderGraph::Link(int32_t from, int32_t from_attr, int32_t to, int32_t to_attr)
{
	std::visit(Stage::AddOutgoing{to, to_attr}, _graph[from]);
	std::visit(Stage::AddIncoming{from, from_attr}, _graph[to]);
    std::visit(Stage::Consume{std::visit(Stage::Expose{from_attr}, _graph[from]), to_attr}, _graph[to]);
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
            for (auto const child : v) 
                ImNodes::Link(id++, child << 8, child);
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
