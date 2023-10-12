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

Game::Game() 
{
	int constexpr width{ 800 };
	int constexpr height{ 600 };
	Win32::Init(width, height);
	Renderer::Init(width, height, Win32::GetNativeWnd());
	ImGuiRenderer::Init(Renderer::Context());
	Camera::Init();

	Clk::Init();

	_sphere = std::make_unique<Sphere>(Renderer::Device(), Renderer::Context());
	_triangle = std::make_unique<Triangle>(Renderer::Device(), Renderer::Context());
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
	_sphere->Update(Renderer::Context(), dt);
	_triangle->Update(Renderer::Context(), dt);
}

void Game::Render()
{
	Renderer::BeginFrame();
	//_triangle->Draw(Renderer::Context());
	_sphere->Draw(Renderer::Context());
	Renderer::EndFrame();
}
