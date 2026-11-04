/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "RHI_Macros.h"
#include "RHI_Types.h"
#include <cstddef>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

enum BufferUsage : uint32_t
{
	BufferUsage_Default = 0,
	BufferUsage_Vertex = 1u << 0,
	BufferUsage_Index = 1u << 1,
	BufferUsage_Dynamic = 1u << 2,
	BufferUsage_Readback = 1u << 3,
};

enum class IndexFormat : uint32_t
{
	UInt16 = 0,
	UInt32,
};

inline uint32_t IndexFormatSize(IndexFormat f)
{
	return (f == IndexFormat::UInt32) ? 4u : 2u;
}

struct BufferDesc
{
	uint64_t Size = 0;
	uint32_t Stride = 0;
	IndexFormat IdxFormat = IndexFormat::UInt16;
	uint32_t Usage = BufferUsage_Default;
	const char* DebugName = nullptr;

	static BufferDesc Vertex(uint64_t size, uint32_t stride, uint32_t usage = BufferUsage_Default)
	{
		BufferDesc d;
		d.Size = size; d.Stride = stride;
		d.Usage = usage | BufferUsage_Vertex;
		return d;
	}

	static BufferDesc Index(uint64_t size, IndexFormat fmt = IndexFormat::UInt16, uint32_t usage = BufferUsage_Default)
	{
		BufferDesc d;
		d.Size = size; d.IdxFormat = fmt;
		d.Usage = usage | BufferUsage_Index;
		return d;
	}
};

class IBuffer : public IObject
{
public:
	virtual const BufferDesc& GetDesc() const = 0;
	virtual uint64_t GetGPUVirtualAddress() const = 0;
};

class ITexture : public IObject
{
public:
	virtual Format GetFormat() const = 0;
	virtual uint32_t GetWidth()  const = 0;
	virtual uint32_t GetHeight() const = 0;
};

RHI_END
/////////////////////////////////////////////////////////////////
