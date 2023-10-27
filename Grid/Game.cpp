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
#include "PipelineStateObject.h"

#include "ViewportStage.h"
#include "CopyStage.h"

#include "VolumeCube.h"
#include "DrawVolumeStage.h"
#include "Texture3DTest.h"
#include "Sourcing3DStage.h"
#include "Advection3DStage.h"
#include "CBFluid.h"
#include "CBTimestep.h"
#include "Initializer3DStage.h"
#include "Pressure3D1DStage.h"
#include "CBPoisson.h"
#include "Divergence3DStage.h"
#include "Poisson3D1DStage.h"
#include "GradientSubtract3DStage.h"

Game::Game() 
{
	int constexpr width{ 1920 };
	int constexpr height{ 1080 };

	gViewportInfo.width = 640;
	gViewportInfo.height = 640;
	gViewportInfo.depth = 640;

	gSimulationInfo.width = 128;
	gSimulationInfo.height = 128;
	gSimulationInfo.depth = 128;

	Win32::Init(width, height);
	Renderer::Init(width, height, gWindowInfo.hWnd);
	Camera::Init();

	Clk::Init();

	_renderGraph.AddStage(std::move(std::make_shared<CBFluid>()));
	_renderGraph.AddStage(std::move(std::make_shared<Sourcing3DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<CBTimestep>()));
	_renderGraph.AddStage(std::move(std::make_shared<Advection3DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<DrawVolumeStage>(Renderer::Context())));
	_renderGraph.AddStage(std::move(std::make_shared<CopyStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<ViewportStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<Initializer3DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Pressure3D1DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBPoisson>(Renderer::Context())));
	_renderGraph.AddStage(std::move(std::make_shared<Divergence3DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Poisson3D1DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<GradientSubtract3DStage>()));

	_renderGraph.Link(0, 256, 1, 0);
	_renderGraph.Link(2, 259, 3, 3);
	_renderGraph.Link(1, 257, 3, 4);
	_renderGraph.Link(1, 258, 3, 5);

	_renderGraph.Link(7, 265, 8, 10);
	_renderGraph.Link(8, 266, 11, 12);

	_renderGraph.Link(3, 260, 10, 11);
	_renderGraph.Link(10, 268, 11, 13);

	_renderGraph.Link(9, 267, 11, 14);

	_renderGraph.Link(11, 269, 12, 15);
	_renderGraph.Link(3, 260, 12, 16);
	_renderGraph.Link(12, 270, 1, 1);

	_renderGraph.Link(3, 261, 1, 2);
	_renderGraph.Link(3, 261, 4, 7);
	_renderGraph.Link(4, 262, 5, 8);
	_renderGraph.Link(5, 263, 6, 9);

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
	_renderGraph.Update(Renderer::Context());
}

void Game::Render()
{
	Renderer::BeginFrame();
	_renderGraph.Run(Renderer::Context());
	Renderer::EndFrame();
}
