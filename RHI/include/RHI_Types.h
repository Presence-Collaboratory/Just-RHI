/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "RHI_Macros.h"
#include <cstdint>
#include <cstddef>
/////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#  if !defined(_WINDOWS_) && !defined(_INC_WINDOWS)
struct HWND__;
typedef struct HWND__* HWND;
#  endif
#endif
/////////////////////////////////////////////////////////////////
RHI_BEGIN

#ifndef RHI_HAVE_EXTERNAL_INT_TYPES
using u8 = std::uint8_t;  using u16 = std::uint16_t;
using u32 = std::uint32_t; using u64 = std::uint64_t;
using s8 = std::int8_t;   using s16 = std::int16_t;
using s32 = std::int32_t;  using s64 = std::int64_t;
#endif

enum class BackendType : uint32_t
{
	DirectX12 = 0,
	DirectX9Ex,
};

enum class DeviceStatus : uint32_t
{
	OK = 0,
	NeedReset,
	Lost,
};

enum class Format : uint32_t
{
	Unknown = 0,
	R8G8B8A8_UNORM,
	R8G8B8A8_UNORM_SRGB,
	B8G8R8A8_UNORM,
	B8G8R8A8_UNORM_SRGB,
	R16G16_FLOAT,
	R32G32_FLOAT,
	R32G32B32_FLOAT,
	R32G32B32A32_FLOAT,
	D24_UNORM_S8_UINT,
	D32_FLOAT,
};

enum class Topology : uint32_t
{
	PointList = 1,
	LineList = 2,
	LineStrip = 3,
	TriangleList = 4,
	TriangleStrip = 5,
};

enum class ResourceState : uint32_t
{
	Common = 0,
	VertexBuffer,
	IndexBuffer,
	ConstantBuffer,
	ShaderResource,
	UnorderedAccess,
	RenderTarget,
	DepthRead,
	DepthWrite,
	CopySource,
	CopyDest,
	Present,
};

struct Viewport
{
	float X = 0.0f, Y = 0.0f;
	float Width = 0.0f, Height = 0.0f;
	float MinZ = 0.0f, MaxZ = 1.0f;
};

struct Rect
{
	s32 Left = 0, Top = 0, Right = 0, Bottom = 0;
};

struct Float4
{
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
};

struct RenderTargetView
{
	static constexpr uint32_t kInvalidSlot = ~0u;
	uint32_t Slot = kInvalidSlot;
	bool IsValid() const { return Slot != kInvalidSlot; }
};

struct DepthStencilView
{
	static constexpr uint32_t kInvalidSlot = ~0u;
	uint32_t Slot = kInvalidSlot;
	bool IsValid() const { return Slot != kInvalidSlot; }
};

class IObject
{
public:
	virtual ~IObject() = default;

	virtual const char* GetDebugName() const { return m_debugName; }
	void SetDebugName(const char* name) { m_debugName = name; }

protected:
	const char* m_debugName = "";
};

RHI_END
/////////////////////////////////////////////////////////////////
