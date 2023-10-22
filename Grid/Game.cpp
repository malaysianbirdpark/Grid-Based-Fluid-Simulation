#include "pch.h"
#include "Game.h"

#include <iostream>

#include "Win32.h"
#include "Renderer.h"
#include "ImGuiRenderer.h"
#include "Camera.h"

#include "imnodes.h"

#include "Transform.h"
#include "Input.h"
#include "Clk.h"

#include "Sphere.h"

#include "DrawStage.h"
#include "BackBufferStage.h"
#include "ViewportStage.h"
#include "Sourcing2DStage.h"
#include "CopyStage.h"
#include "Advection2DStage.h"
#include "CBFluid.h"
#include "CBTimestep.h"
#include "Pressure1DStage.h"
#include "Pressure2DStage.h"
#include "Poisson2D1DStage.h"
#include "Poisson2D2DStage.h"
#include "Poisson2D4DStage.h"
#include "CBPoisson.h"
#include "Divergence2DStage.h"
#include "GradientSubtract2DStage.h"
#include "Initializer2DStage.h"
#include "CBViscosity.h"

Game::Game() 
{
	int constexpr width{ 1920 };
	int constexpr height{ 1080 };
	gViewportInfo.width = 640;
	gViewportInfo.height = 640;
	Win32::Init(width, height);
	Renderer::Init(width, height, gWindowInfo.hWnd);
	Camera::Init();

	Clk::Init();

	_object.push_back(std::move(std::make_shared<Sphere>(Renderer::Context())));

	_renderGraph.AddStage(std::move(std::make_shared<CBFluid>()));
	_renderGraph.AddStage(std::move(std::make_shared<Sourcing2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBTimestep>()));
	_renderGraph.AddStage(std::move(std::make_shared<Advection2DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<CBViscosity>(Renderer::Context())));
	_renderGraph.AddStage(std::move(std::make_shared<Poisson2D2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Poisson2D4DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<Initializer2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Pressure1DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBPoisson>(Renderer::Context())));
	_renderGraph.AddStage(std::move(std::make_shared<Divergence2DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Poisson2D1DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<GradientSubtract2DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<CopyStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<ViewportStage>()));

	// full
	{
        _renderGraph.Link(0, 256, 1, 0);
        _renderGraph.Link(2, 259, 3, 3);
        _renderGraph.Link(1, 257, 3, 4);
        _renderGraph.Link(1, 258, 3, 5);

        _renderGraph.Link(3, 260, 5, 6);
        _renderGraph.Link(3, 260, 5, 7);
        _renderGraph.Link(3, 260, 6, 10);

        _renderGraph.Link(4, 262, 5, 8);
        _renderGraph.Link(4, 262, 6, 11);

        _renderGraph.Link(7, 265, 8, 12);
        _renderGraph.Link(8, 266, 11, 14);

        _renderGraph.Link(5, 263, 10, 13);
        _renderGraph.Link(10, 268, 11, 15);
        _renderGraph.Link(9, 267, 11, 16);

        _renderGraph.Link(11, 269, 12, 17);
        _renderGraph.Link(5, 263, 12, 18);
        _renderGraph.Link(12, 270, 1, 1);

        _renderGraph.Link(13, 271, 14, 20);
        _renderGraph.Link(12, 270, 1, 1);

        _renderGraph.Link(3, 261, 13, 19);
        _renderGraph.Link(3, 261, 1, 2);
	}


	ImNodes::LoadCurrentEditorStateFromIniFile("imnodes_state.ini");
}

Game::~Game()
{
}

int Game::Run()
{
	while (true) {
		if (auto const ecode{ Win32::PumpMessage() }; ecode.has_value()) [[unlikely]] {
			ImNodes::SaveCurrentEditorStateToIniFile("imnodes_state.ini");
			return *ecode;
		}

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
