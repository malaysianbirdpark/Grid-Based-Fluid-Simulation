#include "pch.h"
#include "Game.h"

#include "Win32.h"
#include "Renderer.h"
#include "Camera.h"

#include "Transform.h"

Game::Game() 
{
	int constexpr width{ 800 };
	int constexpr height{ 600 };
	Win32::Init(width, height);
	Renderer::Init(width, height, Win32::GetNativeWnd());
	Camera::Init();

	_sphere = std::make_unique<Sphere>(Renderer::Device());
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

		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput()
{
}

void Game::Update()
{
	_triangle->Update(Renderer::Context(), 1600.0f);
}

void Game::Render()
{
	Renderer::BeginFrame();
	_triangle->Draw(Renderer::Context());
	Renderer::EndFrame();
}
