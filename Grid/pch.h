#pragma once

#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOKERNEL
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define STRICT

#include <Windows.h>

#include <wrl.h>
#include <dxgi.h>
#include <DirectxMath.h>

#include <d3d11.h>
#include <dxgiformat.h>

inline ID3D11Device* pDevice {nullptr};
inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pViewportSRV{ nullptr };

inline struct _WindowInfo {
	int32_t width{};
	int32_t height{};
	HWND hWnd{ nullptr };
	class WindowClass* wndClass{ nullptr };
} gWindowInfo;

inline struct _ViewportInfo {
	int32_t width{};
	int32_t height{};
} gViewportInfo;