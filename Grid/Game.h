#pragma once

#include <memory>

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
};
