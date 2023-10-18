#pragma once

class ImGuiRenderer
{
public: static void Init(ID3D11DeviceContext* context);
public: static void Shutdown();
public: static void BeginFrame();
public: static void EndFrame();

private: static void ImGuiShowDockSpace();
private: static void ImGuiShowViewport();

public:
	inline static bool imguiEnabled{ false };
};


