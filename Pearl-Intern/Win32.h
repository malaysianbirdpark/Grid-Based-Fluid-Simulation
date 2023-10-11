#pragma once

#include <string>
#include <optional>

class Win32 {
public:
    class WindowClass {
    public:
        WindowClass() noexcept;
        ~WindowClass();

        void Shutdown();

        WindowClass(WindowClass const&) = delete;
        WindowClass& operator=(WindowClass const&) = delete;

        [[nodiscard]] char const* GetName() const { return _name.c_str(); }
        [[nodiscard]] HINSTANCE GetInstance() const { return hInst; }
    private:
        std::string _name;
        HINSTANCE hInst;
    };
public:
    static void Init(int const width, int const height, char const* name = "Game");
    static void Shutdown();

    static std::optional<int> PumpMessage();

    [[nodiscard]] static HWND GetNativeWnd() { return _wndData.hWnd; }
    [[nodiscard]] static int GetWidth() { return _wndData.width; }
    [[nodiscard]] static int GetHeight() { return _wndData.height; }

    static void SetWindowSize(int width, int height);
    static void SetViewportSize(int width, int height);
private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
    struct win32_data {
        int width{ 0 };
        int height{ 0 };
        WindowClass* wndClass;
        HWND hWnd{ nullptr };
    };

    inline static win32_data _wndData{};
};

