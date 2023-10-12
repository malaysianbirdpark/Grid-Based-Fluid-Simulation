#pragma once

#include <memory>

#include "Sphere.h"
#include "Triangle.h"
#include "Transform.h"
#include "PipelineStateObject.h"
#include "PSTextures.h"

class Game {
public:  explicit  Game();
public:            ~Game();
public:  int       Run();
private: void      ProcessInput(float const dt);
private: void      Update(float const dt);
private: void      Render();
private:
	std::unique_ptr<Sphere> _sphere;
	std::unique_ptr<Triangle> _triangle;
};
