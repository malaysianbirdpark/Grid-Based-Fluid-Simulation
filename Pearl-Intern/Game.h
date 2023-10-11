#pragma once

#include <memory>

#include "Sphere.h"
#include "Triangle.h"
#include "Transform.h"
#include "PipelineStateObject.h"

class Game {
public:  explicit  Game();
public:            ~Game();
public:  int       Run();
private: void      ProcessInput();
private: void      Update();
private: void      Render();
private:
	std::unique_ptr<Sphere> _sphere;
	std::unique_ptr<Triangle> _triangle;
};
