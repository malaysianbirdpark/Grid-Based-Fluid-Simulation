#pragma once

class ImGuiRenderer
{
public: static void Init(ID3D11DeviceContext& context);
public: static void Shutdown();
public: static void BeginFrame();
public: static void EndFrame();

public:
	inline static bool imguiEnabled{ false };
private:
	inline static int width;
	inline static int height;
};

