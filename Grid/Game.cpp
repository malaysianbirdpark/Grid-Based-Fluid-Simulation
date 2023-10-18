#include "pch.h"
#include "Game.h"

#include <iostream>

#include "Win32.h"
#include "Renderer.h"
#include "ImGuiRenderer.h"
#include "Camera.h"

#include "Transform.h"
#include "Input.h"
#include "Clk.h"

#include "Sphere.h"

#include "DrawStage.h"
#include "BackBufferStage.h"
#include "ViewportStage.h"
#include "Velocity2DStage.h"
#include "Quantity2DStage.h"
#include "Sourcing2DStage.h"
#include "CopyStage.h"
#include "Advection2DStage.h"
#include "CBFluidColor.h"
#include "CBTimestep.h"
#include "Poisson2DStage.h"
#include "Pressure2DStage.h"
#include "Pressure1DStage.h"
#include "CBPoisson.h"
#include "Divergence2DStage.h"
#include "GradientSubtract2DStage.h"

Game::Game() 
{
	int constexpr width{ 1920 };
	int constexpr height{ 1080 };
	gViewportInfo.width = 600;
	gViewportInfo.height = 600;
	Win32::Init(width, height);
	Renderer::Init(width, height, gWindowInfo.hWnd);
	Camera::Init();

	Clk::Init();

	_object.push_back(std::move(std::make_shared<Sphere>(Renderer::Context())));

	_renderGraph.AddStage(std::move(std::make_shared<ViewportStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Sourcing2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Velocity2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CopyStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Quantity2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Quantity2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Advection2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBFluidColor>()));
	_renderGraph.AddStage(std::move(std::make_shared<Velocity2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBTimestep>()));
	_renderGraph.AddStage(std::move(std::make_shared<Poisson2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Pressure2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Pressure1DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBPoisson>(
		Renderer::Context(), 
		- (1.0f / gViewportInfo.width) * (1.0f / gViewportInfo.width), 
	    4))
	);
	_renderGraph.AddStage(std::move(std::make_shared<Divergence2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<GradientSubtract2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Velocity2DStage>()));

	//_renderGraph.Link(7, 265, 1, 1);
	//_renderGraph.Link(1, 257, 2, 2);
	//_renderGraph.Link(1, 258, 4, 4);
	//_renderGraph.Link(9, 267, 6, 6);
	//_renderGraph.Link(2, 259, 6, 7);
	//_renderGraph.Link(4, 261, 6, 8);
	//_renderGraph.Link(6, 263, 8, 9);
	//_renderGraph.Link(6, 264, 5, 5);
	//_renderGraph.Link(11, 269, 10, 10);
	//_renderGraph.Link(8, 266, 14, 15);
	//_renderGraph.Link(14, 272, 10, 11);
	//_renderGraph.Link(10, 268, 12, 14);
	//_renderGraph.Link(13, 271, 10, 12);
	//_renderGraph.Link(12, 270, 15, 16);
	//_renderGraph.Link(8, 266, 15, 17);
	//_renderGraph.Link(15, 273, 16, 18);

	//_renderGraph.Link(5, 262, 3, 3);
	//_renderGraph.Link(3, 260, 0, 0);
}

Game::~Game()
{
}

int Game::Run()
{
	while (true) {
		if (auto const ecode{ Win32::PumpMessage() }; ecode.has_value()) [[unlikely]]
			return *ecode;

		auto const dt{ Clk::Mark() };

		ProcessInput(dt);
		Update(dt);
		Render();
	}
}

void Game::ProcessInput(float const dt)
{
	auto const& kbd_tracker{ Input::GetKeyboardState() };

	if (kbd_tracker.pressed.Escape)
		Input::ToggleRaw();

	auto const kbd_state{ kbd_tracker.GetLastState() };
	if (!Input::_cursorEnabled) {
		DirectX::XMFLOAT3 translate{};
		translate.x += static_cast<float>(kbd_state.D) * dt;
		translate.x -= static_cast<float>(kbd_state.A) * dt;
		translate.y += static_cast<float>(kbd_state.R) * dt;
		translate.y -= static_cast<float>(kbd_state.F) * dt;
		translate.z += static_cast<float>(kbd_state.W) * dt;
		translate.z -= static_cast<float>(kbd_state.S) * dt;

		Camera::Translate(translate);

		while (auto const raw_delta{ Input::ReadRawDelta() })
			Camera::Rotate(raw_delta.value().first * dt, raw_delta.value().second * dt);
	}
}

void Game::Update(float const dt)
{
	for (auto& obj : _object)
		obj->Update(Renderer::Context(), dt);
	_renderGraph.Update();
}

void Game::Render()
{
	Renderer::BeginFrame();
	_renderGraph.Run(Renderer::Context());
	Renderer::EndFrame();
}
