/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "RHI_Macros.h"
#include "RHI_Types.h"
#include <vector>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

struct VertexElement
{
	const char* SemanticName = "POSITION";
	uint32_t SemanticIndex = 0;
	Format Format = Format::R32G32B32_FLOAT;
	uint32_t Offset = 0;
	uint32_t InputSlot = 0;
};

struct PipelineStateDesc
{
	const void* VS_Bytecode = nullptr;
	size_t VS_Size = 0;
	const void* PS_Bytecode = nullptr;
	size_t PS_Size = 0;

	const VertexElement* VertexElements = nullptr;
	uint32_t VertexElementCount = 0;
	uint32_t VertexStride = 0;
	Topology Topology = Topology::TriangleList;

	Format RTVFormat = Format::R8G8B8A8_UNORM;
	Format DSVFormat = Format::Unknown;

	bool DepthEnable = false;
	bool CullBackFace = true;
	bool Wireframe = false;

	const char* DebugName = "PSO";
};

class IPipelineState : public IObject
{
public:
	virtual const PipelineStateDesc& GetDesc() const = 0;
	virtual void* GetNative() = 0;
};

RHI_END
/////////////////////////////////////////////////////////////////
