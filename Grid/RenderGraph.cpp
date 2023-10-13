#include "pch.h"
#include "RenderGraph.h"

#include <queue>

RenderGraph::RenderGraph()
{
}

void RenderGraph::Run(ID3D11DeviceContext& context)
{
    for (auto i{ 0 }; i != _visited.size(); ++i)
        _visited[i] = false;

    auto bfs_queue{ std::queue<int32_t> {} };
    bfs_queue.emplace(0);
    _visited[0] = true;
    while (!bfs_queue.empty()) {
        auto const cur_stage{ bfs_queue.front() };
        bfs_queue.pop();

        std::visit(Stage::Run{ context }, _graph[cur_stage]);

        for (auto const child : std::visit(Stage::GetChilds{}, _graph[cur_stage])) {
            if (!_visited[child]) {
                _visited[child] = true;
                bfs_queue.emplace(child);
            }
        }
    }
}

void RenderGraph::InsertStageAsChild(int32_t parent_id, Stage::Stage stage)
{
    _graph.emplace_back(std::move(stage));
    _visited.emplace_back(false);

    auto id{ 0 };
    if (parent_id != -1) [[likely]] {
		id =  static_cast<int32_t>(_graph.size() - 1);
		std::visit(Stage::AddChild{id}, _graph[parent_id]);
		std::visit(Stage::SetParent{ parent_id }, _graph.back());
    }

    std::visit(Stage::SetID{ id }, _graph.back());
}

void RenderGraph::InsertStageAsSibling(int32_t sibling_id, Stage::Stage stage)
{
    auto const parent_id{ std::visit(Stage::GetParent{}, _graph[sibling_id]) };
    if (parent_id == -1)
        return;

    auto const id{ static_cast<int32_t>(_graph.size()) };
    _graph.emplace_back(std::move(stage));
    _visited.emplace_back(false);
    std::visit(Stage::SetID{ id }, _graph.back());
    std::visit(Stage::AddChild{id}, _graph[parent_id]);
    std::visit(Stage::SetParent{ parent_id }, _graph.back());
}
