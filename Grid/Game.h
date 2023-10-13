#pragma once

#include <memory>

#include "RenderObject.h"
#include "Transform.h"
#include "PipelineStateObject.h"
#include "PSTextures.h"

#include "RenderGraph.h"

class Game {
public:  explicit  Game();
public:            ~Game();
public:  int       Run();
private: void      ProcessInput(float const dt);
private: void      Update(float const dt);
private: void      Render();
private:
	RenderGraph _renderGraph;
	std::vector<std::shared_ptr<RenderObject>> _object;
};
