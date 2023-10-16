#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "Stage.h"

class RenderGraph {
public:  explicit RenderGraph();
public:  void     Run(ID3D11DeviceContext& context);
public:  void     Feed(ID3D11DeviceContext& context, int32_t feeder, int32_t consumer);
public:  void     Consume(ID3D11DeviceContext& context, int32_t feeder, int32_t consumer);
public:  void     AddStage(Stage::Stage stage);
public:  void     Link(int32_t from, int32_t from_attr, int32_t to, int32_t to_attr);
public:  void     ImGuiShowRenderGraphEditWindow();
private: void     RecalculateTopology();
private:
	std::vector<Stage::Stage>             _graph;
	std::vector<int32_t>                  _orderOfExecution;
	std::unordered_map<int32_t, int32_t>  _indegree;
	std::vector<std::vector<int32_t>>     _links;
};

