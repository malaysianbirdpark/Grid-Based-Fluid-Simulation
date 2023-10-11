#pragma once

class ImGuiRenderer
{
public:
	static void Init(ID3D11DeviceContext& context);
	static void Shutdown();

	static void BeginFrame();
	static void EndFrame();
public:
	inline static bool imguiEnabled{ false };
private:
	inline static int width;
	inline static int height;
};

