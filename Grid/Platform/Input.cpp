#include "pch.h"
#include "Input.h"

void Input::Init(HWND hWnd)
{
    _hWnd = hWnd;
    _mouse.SetWindow(hWnd);
}

DirectX::Keyboard::KeyboardStateTracker& Input::GetKeyboardState() {
    _tracker.Update(_keyboard.GetState());
    return _tracker;
}

DirectX::Mouse::State Input::GetMouseState() {
    return _mouse.GetState();
}

std::optional<std::pair<int, int>> Input::ReadRawDelta()
{
    if (_rawDeltaBuffer.empty())
        return std::nullopt;
    auto const d{ _rawDeltaBuffer.front() };
    _rawDeltaBuffer.pop();
    return std::pair{ d.x, d.y };
}

void Input::ToggleActivation(UINT flag)
{
    if (_cursorEnabled) {
        if (flag) {
            ConfineCursor();
            HideCursor();
        }
        else {
            FreeCursor();
            ShowCursor();
        }
    }
}

void Input::KeyboardButtonDown()
{
    if (!_cursorEnabled) {
        ConfineCursor();
        HideCursor();
    }
}

void Input::ProcessRawInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!_rawEnabled)
        return;

    UINT size;
    // first get the size of the input data
    if (GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lParam),
        RID_INPUT,
        nullptr,
        &size,
        sizeof(RAWINPUTHEADER)) == -1)
    {
        return;
    }
    _rawBuffer.resize(size);
    if (GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lParam),
        RID_INPUT,
        _rawBuffer.data(),
        &size,
        sizeof(RAWINPUTHEADER)) == -1)
    {
        return;
    }

    // process the data
    auto& ri{ reinterpret_cast<RAWINPUT const&>(*_rawBuffer.data()) };
    if (ri.header.dwType == RIM_TYPEMOUSE && ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY)
        _rawDeltaBuffer.push<RawMouseData>(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
}

void Input::ToggleRaw()
{
    if (_cursorEnabled) {
        DisableCursor();
        _rawEnabled = true;
    }
    else
    {
        EnableCursor();
        _rawEnabled = false;
    }
}

void Input::EnableCursor()
{
    _cursorEnabled = true;
    ShowCursor();
    //EnableImGuiMouse();
    FreeCursor();
}

void Input::DisableCursor()
{
    _cursorEnabled = false;
    HideCursor();
    //DisableImGuiMouse();
    ConfineCursor();
}

void Input::ClearKeyboard()
{
}

void Input::ClearMouse()
{
}

void Input::ConfineCursor()
{
    RECT r{};
    GetClientRect(_hWnd, &r);
    MapWindowPoints(_hWnd, nullptr, reinterpret_cast<POINT*>(&r), 2);
    ClipCursor(&r);
}

void Input::HideCursor()
{
    while (::ShowCursor(FALSE) >= 0)
        ;
}

void Input::FreeCursor()
{
    ClipCursor(nullptr);

}

void Input::ShowCursor()
{
    while (::ShowCursor(TRUE) < 0)
        ;
}
