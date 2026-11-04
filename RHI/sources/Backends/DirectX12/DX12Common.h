/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <string>
/////////////////////////////////////////////////////////////////
using Microsoft::WRL::ComPtr;
/////////////////////////////////////////////////////////////////
#define DX_CHECK(hr)                                                       \
	do {                                                                   \
		HRESULT _hr = (hr);                                                \
		if (FAILED(_hr)) {                                                 \
			char _buf[256];                                                \
			sprintf_s(_buf, "%s(%d): HRESULT 0x%08X",                      \
					  __FILE__, __LINE__, (unsigned)_hr);                  \
			MessageBoxA(nullptr, _buf, "DX12 Error", MB_ICONERROR);        \
			__debugbreak();                                                \
		}                                                                  \
	} while (0)

#define DX_CHECK_MSG(hr, msg)                                              \
	do {                                                                   \
		HRESULT _hr = (hr);                                                \
		if (FAILED(_hr)) {                                                 \
			char _buf[512];                                                \
			sprintf_s(_buf, "%s: 0x%08X\n%s", (msg), (unsigned)_hr,        \
					  __FILE__);                                           \
			MessageBoxA(nullptr, _buf, "DX12 Error", MB_ICONERROR);        \
			__debugbreak();                                                \
		}                                                                  \
	} while (0)
/////////////////////////////////////////////////////////////////
