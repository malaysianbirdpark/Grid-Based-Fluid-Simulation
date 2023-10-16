#include "pch.h"
#include "ImGuiRenderer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imnodes.h"

#include "Win32.h"
#include "Renderer.h"

#include "RenderGraph.h"

void ImGuiRenderer::Init(ID3D11DeviceContext& context)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Win32::GetNativeWnd());
    ImGui_ImplDX11_Init(&Renderer::Device(), &context);

    imguiEnabled = true;
    width = Win32::GetWidth();
    height = Win32::GetHeight();
}

void ImGuiRenderer::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
}

void ImGuiRenderer::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (ImGui::Begin("System Info")) {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();

    bool showDemoWindow{ true };
    if (showDemoWindow) [[likely]] {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }
}

void ImGuiRenderer::EndFrame()
{
    auto& io{ ImGui::GetIO() };
    io.DisplaySize = ImVec2(
        static_cast<float>(width),
        static_cast<float>(height)
    );


    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

int32_t ImGuiNodeManager::IssueNodeID() {
    return _nodeID++;
}

int32_t ImGuiNodeManager::IssueAttrID() {
    return _attrID++;
}

int32_t ImGuiNodeManager::LastNodeID() {
    return _nodeID - 1;
}

int32_t ImGuiNodeManager::LastAttrID() {
    return _attrID - 1;
}
