/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "DX12Common.h"
#include <include/RHI_CommandList.h>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

class DX12Device;

class DX12CommandList : public ICommandList
{
public:
	DX12CommandList(DX12Device* device, ID3D12Device* nativeDevice);

	void SetCurrentAllocator(ID3D12CommandAllocator* alloc) { m_allocator = alloc; }

	void Begin() override;
	void End() override;

	void Transition(ITexture* tex, ResourceState newState) override;

	void SetRenderTarget(RenderTargetView rtv) override;
	void ClearRenderTarget(RenderTargetView rtv, const Float4& color) override;
	void SetViewport(const Viewport& vp) override;
	void SetScissorRect(const Rect& rect) override;

	void SetPipelineState(IPipelineState* pso) override;
	void SetPrimitiveTopology(Topology topo) override;
	void SetVertexBuffer(IBuffer* vb, uint32_t offset = 0) override;
	void SetIndexBuffer(IBuffer* ib, IndexFormat fmt) override;

	void Draw(uint32_t vertexCount, uint32_t startVertex = 0) override;
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, int32_t baseVertex = 0) override;

	void* GetNative() override { return m_cmd.Get(); }

	ID3D12GraphicsCommandList* GetCommandList() const { return m_cmd.Get(); }
	ID3D12CommandAllocator* GetAllocator() const { return m_allocator.Get(); }

private:
	void AddTransition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	DX12Device* m_device = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_cmd;
	ComPtr<ID3D12CommandAllocator> m_allocator;
	bool m_open = false;
};

RHI_END
/////////////////////////////////////////////////////////////////
