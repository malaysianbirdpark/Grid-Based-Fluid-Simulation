#pragma once

#include <vector>
#include <memory>

#include "Stage.h"

class RenderGraph {
public:  explicit RenderGraph();
public:  void     Run(ID3D11DeviceContext& context);
public:  void     InsertStageAsChild(int32_t parent_id, Stage::Stage stage);
public:  void     InsertStageAsSibling(int32_t sibling_id, Stage::Stage stage);
public:  void     ImGuiShowRenderGraphEditWindow();
private:
	std::vector<Stage::Stage>          _graph;
	std::vector<bool>                  _visited;
	std::vector<std::vector<int32_t>>  _links;
};

