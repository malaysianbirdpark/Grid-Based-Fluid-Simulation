#pragma once

#include <optional>
#include <vector>

#include "DirectXTK-main/Inc/Mouse.h"
#include "DirectXTK-main/Inc/Keyboard.h"

#include "CircularQueue.h"

class Input
{
public:
    static void Init(HWND hWnd);
public:
    [[nodiscard]] static DirectX::Keyboard::KeyboardStateTracker& GetKeyboardState();
    [[nodiscard]] static DirectX::Mouse::State GetMouseState();
    [[nodiscard]] static std::optional<std::pair<int, int>> ReadRawDelta();

    static void ToggleActivation(UINT flag);
    static void KeyboardButtonDown();

    static void ProcessRawInput(UINT msg, WPARAM wParam, LPARAM lParam);

    static void ToggleRaw();

    static void EnableCursor();
    static void DisableCursor();
    static void ClearKeyboard();
    static void ClearMouse();
private:
    static void ConfineCursor();
    static void HideCursor();
    static void FreeCursor();
    static void ShowCursor();

    static void EnableImGuiMouse();
    static void DisableImGuiMouse();
private:
    struct RawMouseData {
        int x;
        int y;
    };
private:
    inline static DirectX::Mouse _mouse{};
    inline static DirectX::Keyboard _keyboard{};
    inline static DirectX::Keyboard::KeyboardStateTracker _tracker{};

    inline static CircularQueue<RawMouseData, 16u> _rawDeltaBuffer;
    inline static std::vector<BYTE>                _rawBuffer;

    inline static HWND _hWnd{ nullptr };
public:
    inline static bool _cursorEnabled{ true };
    inline static bool _rawEnabled{ false };
};

