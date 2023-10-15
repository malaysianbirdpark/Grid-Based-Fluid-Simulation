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

#include "ComputeStageBuilders.h"

Game::Game() 
{
	int constexpr width{ 800 };
	int constexpr height{ 600 };
	Win32::Init(width, height);
	Renderer::Init(width, height, Win32::GetNativeWnd());
	ImGuiRenderer::Init(Renderer::Context());
	Camera::Init();

	Clk::Init();

	_object.push_back(std::move(std::make_shared<Sphere>(Renderer::Device(), Renderer::Context())));

	//_renderGraph.InsertStageAfter(-1, TestCS(Renderer::Device(), Renderer::SwapChain()));
	_renderGraph.InsertStageAfter(0, std::move(std::make_shared<DrawStage>("Earth", _object.back())));
	_renderGraph.InsertStageAfter(0, TestCS1(Renderer::Device(), Renderer::SwapChain()));
	_renderGraph.InsertStageAfter(1, TestCS2(Renderer::Device(), Renderer::SwapChain()));
	_renderGraph.InsertStageAfter(1, StableFluid2D_Sourcing(Renderer::Device()));

	Microsoft::WRL::ComPtr<ID3D11Resource> back_buffer {};
	Renderer::SwapChain().GetBuffer(0u, IID_PPV_ARGS(back_buffer.GetAddressOf()));
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
}

void Game::Render()
{
	Renderer::BeginFrame();
	_renderGraph.Run(Renderer::Context());
	Renderer::EndFrame();
}
