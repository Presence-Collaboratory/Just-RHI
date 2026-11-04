/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#include "DX12CommandList.h"
#include "DX12Device.h"
#include "DX12Conversions.h"
#include "DX12PipelineState.h"
#include "DX12Resources.h"
/////////////////////////////////////////////////////////////////
RHI_BEGIN

DX12CommandList::DX12CommandList(DX12Device* device, ID3D12Device* nativeDevice) : m_device(device)
{
	DX_CHECK(nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocator)));

	DX_CHECK(nativeDevice->CreateCommandList(0, 
											 D3D12_COMMAND_LIST_TYPE_DIRECT,
											 m_allocator.Get(), 
											 nullptr,
											 IID_PPV_ARGS(&m_cmd)));

	DX_CHECK(m_cmd->Close());
}

void DX12CommandList::Begin()
{
	if(m_open)
		return;

	if(!m_allocator)
	{
		OutputDebugStringA("! [DX12] Begin(): no allocator bound (BeginFrame not called?)\n");
		return;
	}

	DX_CHECK(m_allocator->Reset());
	DX_CHECK(m_cmd->Reset(m_allocator.Get(), nullptr));
	m_open = true;
}

void DX12CommandList::End()
{
	if(!m_open)
		return;
	DX_CHECK(m_cmd->Close());
	m_open = false;
}

void DX12CommandList::AddTransition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	if(before == after)
		return;

	D3D12_RESOURCE_BARRIER b = {};
	b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	b.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	b.Transition.pResource = resource;
	b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	b.Transition.StateBefore = before;
	b.Transition.StateAfter = after;

	m_cmd->ResourceBarrier(1, &b);
}

void DX12CommandList::Transition(ITexture* tex, ResourceState newState)
{
	auto* t = static_cast<DX12Texture*>(tex);
	D3D12_RESOURCE_STATES after = ToD3D12State(newState);
	AddTransition(t->GetNative(), t->GetCurrentState(), after);
	t->SetCurrentState(after);
}

void DX12CommandList::SetRenderTarget(RenderTargetView rtv)
{
	if(!rtv.IsValid())
		return;
	D3D12_CPU_DESCRIPTOR_HANDLE h = m_device->GetRTVHandle(rtv.Slot);
	m_cmd->OMSetRenderTargets(1, &h, FALSE, nullptr);
}

void DX12CommandList::ClearRenderTarget(RenderTargetView rtv,
										const Float4& color)
{
	if(!rtv.IsValid())
		return;
	D3D12_CPU_DESCRIPTOR_HANDLE h = m_device->GetRTVHandle(rtv.Slot);
	m_cmd->ClearRenderTargetView(h, &color.x, 0, nullptr);
}

void DX12CommandList::SetViewport(const Viewport& vp)
{
	D3D12_VIEWPORT v = {};
	v.TopLeftX = vp.X;
	v.TopLeftY = vp.Y;
	v.Width = vp.Width;
	v.Height = vp.Height;
	v.MinDepth = vp.MinZ;
	v.MaxDepth = vp.MaxZ;
	m_cmd->RSSetViewports(1, &v);
}

void DX12CommandList::SetScissorRect(const Rect& rect)
{
	D3D12_RECT r = {rect.Left, rect.Top, rect.Right, rect.Bottom};
	m_cmd->RSSetScissorRects(1, &r);
}

void DX12CommandList::SetPipelineState(IPipelineState* pso)
{
	auto* p = static_cast<DX12PipelineState*>(pso);
	m_cmd->SetPipelineState(p->GetPSO());
	m_cmd->SetGraphicsRootSignature(p->GetRootSignature());
}

void DX12CommandList::SetPrimitiveTopology(Topology topo)
{
	m_cmd->IASetPrimitiveTopology(ToD3D12Topology(topo));
}

void DX12CommandList::SetVertexBuffer(IBuffer* vb, uint32_t offset)
{
	auto* b = static_cast<DX12Buffer*>(vb);
	AddTransition(b->GetNative(), b->GetCurrentState(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	b->SetCurrentState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	D3D12_VERTEX_BUFFER_VIEW v = {};
	v.BufferLocation = b->GetGPUVirtualAddress() + offset;
	v.SizeInBytes = (UINT)(b->GetDesc().Size - offset);
	v.StrideInBytes = b->GetDesc().Stride;

	m_cmd->IASetVertexBuffers(0, 1, &v);
}

void DX12CommandList::SetIndexBuffer(IBuffer* ib, IndexFormat fmt)
{
	auto* b = static_cast<DX12Buffer*>(ib);
	AddTransition(b->GetNative(), b->GetCurrentState(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
	b->SetCurrentState(D3D12_RESOURCE_STATE_INDEX_BUFFER);

	D3D12_INDEX_BUFFER_VIEW v = {};
	v.BufferLocation = b->GetGPUVirtualAddress();
	v.SizeInBytes = (UINT)b->GetDesc().Size;
	v.Format = (fmt == IndexFormat::UInt32) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

	m_cmd->IASetIndexBuffer(&v);
}

void DX12CommandList::Draw(uint32_t vertexCount, uint32_t startVertex)
{
	m_cmd->DrawInstanced(vertexCount, 1, startVertex, 0);
}

void DX12CommandList::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex)
{
	m_cmd->DrawIndexedInstanced(indexCount, 1, startIndex, baseVertex, 0);
}

RHI_END
/////////////////////////////////////////////////////////////////
