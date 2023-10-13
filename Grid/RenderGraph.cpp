#include "pch.h"
#include "RenderGraph.h"

#include <queue>
#include <utility>

#include "imgui.h"
#include "imnodes.h"

RenderGraph::RenderGraph()
{
    InsertStageAsChild(-1, std::make_shared<Stage::RootDummyStage>());
    _links.resize(1);
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

    ImGuiShowRenderGraphEditWindow();
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

    _links.emplace_back();
    if (parent_id != -1)
		_links[parent_id].emplace_back(id);
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

    _links.emplace_back();
	_links[parent_id].emplace_back(id);
}

void RenderGraph::ImGuiShowRenderGraphEditWindow()
{
    if (ImGui::Begin("Render Graph")) {
		ImNodes::BeginNodeEditor();

        // Root
		ImNodes::BeginNode(0);
        ImNodes::BeginNodeTitleBar();
		ImGui::Text(std::visit(Stage::GetStageName{}, _graph[0]));
		ImGui::Text(std::visit(Stage::GetName{}, _graph[0]));
        ImNodes::EndNodeTitleBar();

        {
            auto child_id{ 0 };
			for (auto& child : std::visit(Stage::GetChilds{}, _graph[0])) {
                child_id = std::visit(Stage::GetID{}, _graph[child]);
				ImNodes::BeginOutputAttribute(child_id << 8);
				ImGui::Text("Child %d", child_id);
				ImNodes::EndOutputAttribute();
			}
        }

		ImNodes::EndNode();

        for (auto node_id{ 1 }; node_id < _graph.size(); ++node_id) {
			ImNodes::BeginNode(node_id);
			ImNodes::BeginNodeTitleBar();
            ImGui::Text(std::visit(Stage::GetStageName{}, _graph[node_id]));
            ImGui::Text(std::visit(Stage::GetName{}, _graph[node_id]));
			ImNodes::EndNodeTitleBar();

            ImNodes::BeginInputAttribute(node_id);
            ImGui::Text("Parent");
            ImNodes::EndInputAttribute();

            auto child_id{ 0 };
			for (auto& child : std::visit(Stage::GetChilds{}, _graph[node_id])) {
                child_id = std::visit(Stage::GetID{}, _graph[child]);
				ImNodes::BeginOutputAttribute(child_id << 8);
				ImGui::Text("Child %d", child_id++);
				ImNodes::EndOutputAttribute();
			}

			ImNodes::EndNode();
        }

        auto id{ 0 };
        for (auto const& v : _links) {
            for (auto const child : v) 
                ImNodes::Link(id++, child << 8, child);
        }

		ImNodes::EndNodeEditor();
    }
	ImGui::End();
}
