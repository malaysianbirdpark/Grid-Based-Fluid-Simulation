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

#include "RenderObject.h"
#include "Transform.h"
#include "PipelineStateObject.h"
#include "PSTextures.h"

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

#include "VolumeCube.h"

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

	_renderGraph.AddStage(std::move(std::make_shared<DrawStage>("VolumeCube", std::make_shared<VolumeCube>(Renderer::Context()))));
	_renderGraph.AddStage(std::move(std::make_shared<CopyStage>()));
	_renderGraph.AddStage(std::move(std::make_shared<ViewportStage>()));

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
