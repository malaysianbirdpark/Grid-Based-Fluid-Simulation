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
#include "Diffusion3DStage.h"
#include "PressureProjection3DStage.h"
#include "CBViscosity.h"
#include "MCAdvection3DStage.h"
#include "ExternalForces3DStage.h"

#include "DrawSceneStage.h"
#include "PipelineStateObject.h"

Game::Game() 
{
	int constexpr width{ 1920 };
	int constexpr height{ 1080 };

	gWindowInfo.width = 1920;
	gWindowInfo.height = 1080;

	gViewportInfo.width = 720;
	gViewportInfo.height = 480;
	gViewportInfo.depth = 480;

	gSimulationInfo.width = 128;
	gSimulationInfo.height = 128;
	gSimulationInfo.depth = 128;

	Win32::Init(width, height);
	Renderer::Init(width, height, gWindowInfo.hWnd);
	Camera::Init();

	Clk::Init();

	_renderGraph.AddStage(std::move(std::make_shared<DrawSceneStage>(Renderer::Context(), "Scene")));

	_renderGraph.AddStage(std::move(std::make_shared<CBFluid>()));
	_renderGraph.AddStage(std::move(std::make_shared<Sourcing3DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<CBTimestep>()));
	_renderGraph.AddStage(std::move(std::make_shared<MCAdvection3DStage>()));
	//_renderGraph.AddStage(std::move(std::make_shared<Advection3DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<ExternalForces3DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<Initializer3DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Pressure3D1DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<CBPoisson>(Renderer::Context())));
	_renderGraph.AddStage(std::move(std::make_shared<Divergence3DStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<Poisson3D1DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<PressureProjection3DStage>()));

	//_renderGraph.AddStage(std::move(std::make_shared<CBViscosity>(Renderer::Context())));
	//_renderGraph.AddStage(std::move(std::make_shared<Diffusion3DStage>()));

	_renderGraph.AddStage(std::move(std::make_shared<DrawVolumeStage>(Renderer::Context())));
	_renderGraph.AddStage(std::move(std::make_shared<CopyStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<ViewportStage>()));

	_renderGraph.Link(0, 256, 12, 15);

	_renderGraph.Link(1, 257, 2, 1);

	_renderGraph.Link(2, 258, 4, 5);
	_renderGraph.Link(2, 259, 4, 6);

	_renderGraph.Link(3, 260, 4, 4);

	_renderGraph.Link(4, 261, 5, 7);
	_renderGraph.Link(4, 262, 12, 16);
	_renderGraph.Link(4, 262, 2, 3);

	_renderGraph.Link(5, 263, 9, 9);
	_renderGraph.Link(5, 263, 11, 14);

	_renderGraph.Link(6, 264, 7, 8);

	_renderGraph.Link(7, 265, 10, 10);

	_renderGraph.Link(8, 266, 10, 12);

	_renderGraph.Link(9, 267, 10, 11);

	_renderGraph.Link(10, 268, 11, 13);

	_renderGraph.Link(11, 269, 2, 2);

	_renderGraph.Link(12, 270, 13, 17);

	_renderGraph.Link(13, 271, 14, 18);

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
		Render(dt);
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

void Game::Render(float const dt)
{
	Renderer::BeginFrame();
	_renderGraph.Run(Renderer::Context());
	Update(dt);
	Renderer::EndFrame();
}
