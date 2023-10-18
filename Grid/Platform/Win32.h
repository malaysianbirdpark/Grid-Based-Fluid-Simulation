#pragma once

#include <string>
#include <optional>

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

class Win32 {
	friend class WindowClass;
public:
    static void Init(int const width, int const height, char const* name = "Game");
    static void Shutdown();

    static std::optional<int> PumpMessage();

    static void SetWindowSize(int width, int height);
    static void SetViewportSize(int width, int height);
private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
};

