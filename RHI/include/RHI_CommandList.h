/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "RHI_Macros.h"
#include "RHI_Types.h"
#include "RHI_Resources.h"
/////////////////////////////////////////////////////////////////
RHI_BEGIN

class IPipelineState;

class ICommandList
{
public:
	virtual ~ICommandList() = default;

	virtual void Begin() = 0;
	virtual void End() = 0;

	virtual void Transition(ITexture* tex, ResourceState newState) = 0;

	virtual void SetRenderTarget(RenderTargetView rtv) = 0;
	virtual void ClearRenderTarget(RenderTargetView rtv, const Float4& color) = 0;

	virtual void SetViewport(const Viewport& vp) = 0;
	virtual void SetScissorRect(const Rect& rect) = 0;

	virtual void SetPipelineState(IPipelineState* pso) = 0;
	virtual void SetPrimitiveTopology(Topology topo) = 0;
	virtual void SetVertexBuffer(IBuffer* vb, uint32_t offset = 0) = 0;
	virtual void SetIndexBuffer(IBuffer* ib, IndexFormat fmt) = 0;

	virtual void Draw(uint32_t vertexCount, uint32_t startVertex = 0) = 0;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, int32_t baseVertex = 0) = 0;

	virtual void* GetNative() = 0;
};

RHI_END
/////////////////////////////////////////////////////////////////
