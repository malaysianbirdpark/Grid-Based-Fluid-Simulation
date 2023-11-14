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
#include "DirLight.h"
#include "PointLight.h"

#include "VoxelizationStage.h"
#include "CBVoxelization.h"

#include "CBDimension.h"

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

    _dirLight = std::make_unique<DirLight>();
    _pointLight = std::make_unique<PointLight>();

	_voxeler = std::make_unique<VoxelizationStage>(Renderer::Context());
	_dimension = std::make_unique<CBDimension>(Renderer::Context());

	std::shared_ptr<DrawSceneStage> _target{std::make_shared<DrawSceneStage>(Renderer::Context(), "Scene")};
	_smoke.AddStage(_target);
	_voxeler->AddTargetScene(_target);

	_smoke.AddStage(std::move(std::make_shared<Sourcing3DStage>()));

	_smoke.AddStage(std::move(std::make_shared<MCAdvection3DStage>()));
	//_smoke.AddStage(std::move(std::make_shared<Advection3DStage>()));

	_smoke.AddStage(std::move(std::make_shared<ExternalForces3DStage>()));

	_smoke.AddStage(std::move(std::make_shared<Divergence3DStage>()));
	_smoke.AddStage(std::move(std::make_shared<Poisson3D1DStage>()));

	_smoke.AddStage(std::move(std::make_shared<PressureProjection3DStage>()));

	//_smoke.AddStage(std::move(std::make_shared<CBViscosity>(Renderer::Context())));
	//_smoke.AddStage(std::move(std::make_shared<Diffusion3DStage>()));

	_smoke.AddStage(std::move(std::make_shared<DrawVolumeStage>(Renderer::Context())));
	_smoke.AddStage(std::move(std::make_shared<CopyStage>()));
	_smoke.AddStage(std::move(std::make_shared<ViewportStage>()));

	_smoke.Link(0, 256, 7, 11);

	_smoke.Link(1, 257, 2, 3);
	_smoke.Link(1, 258, 2, 4);

	_smoke.Link(2, 259, 3, 5);
	_smoke.Link(2, 260, 3, 6);
	_smoke.Link(2, 260, 7, 12);
	_smoke.Link(2, 260, 1, 2);

	_smoke.Link(3, 261, 4, 7);
	_smoke.Link(3, 261, 6, 10);

	_smoke.Link(4, 262, 5, 8);

	_smoke.Link(5, 263, 6, 9);

	_smoke.Link(6, 264, 1, 1);

	_smoke.Link(7, 265, 8, 13);

	_smoke.Link(8, 266, 9, 14);

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
	_smoke.Update(Renderer::Context());
}

void Game::Render(float const dt)
{
	Renderer::BeginFrame();

	_dimension->Run(Renderer::Context());
	_voxeler->Run(Renderer::Context());
    _dirLight->Run(Renderer::Context());
    _pointLight->Run(Renderer::Context());
	_smoke.Run(Renderer::Context());
	Update(dt);
	Renderer::EndFrame();
}
