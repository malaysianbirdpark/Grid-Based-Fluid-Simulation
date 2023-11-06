#pragma once

#include <memory>

#include "RenderGraph.h"

class Game {
public:  explicit  Game();
public:            ~Game();
public:  int       Run();
private: void      ProcessInput(float const dt);
private: void      Update(float const dt);
private: void      Render(float const dt);
private:
	RenderGraph _smoke;
	RenderGraph _water;
    std::unique_ptr<class DirLight>   _dirLight{};
    std::unique_ptr<class PointLight> _pointLight{};
    std::unique_ptr<class VoxelizationStage> _voxeler{};
    std::unique_ptr<class InverseTransform> _hell{};
};
