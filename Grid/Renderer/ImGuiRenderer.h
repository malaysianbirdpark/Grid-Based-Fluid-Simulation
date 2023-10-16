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


class ImGuiNodeManager {
public: static int32_t IssueNodeID();
public: static int32_t IssueAttrID();
public: static int32_t LastNodeID();
public: static int32_t LastAttrID();
private:
	inline static int32_t _nodeID {0};
	inline static int32_t _attrID {0};
};
