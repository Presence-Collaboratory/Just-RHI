/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "DX12Common.h"
#include <include/RHI_Resources.h>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

class DX12Buffer : public IBuffer
{
public:
	DX12Buffer(ID3D12Device* device, const BufferDesc& desc, D3D12_HEAP_TYPE heapType);

	const BufferDesc& GetDesc() const override { return m_desc; }
	uint64_t GetGPUVirtualAddress() const override { return m_gpuAddress; }

	ID3D12Resource* GetNative() const { return m_resource.Get(); }
	D3D12_RESOURCE_STATES GetCurrentState() const { return m_state; }
	void SetCurrentState(D3D12_RESOURCE_STATES s) { m_state = s; }
	D3D12_HEAP_TYPE GetHeapType() const { return m_heapType; }

private:
	BufferDesc m_desc;
	ComPtr<ID3D12Resource> m_resource;
	uint64_t m_gpuAddress = 0;
	D3D12_RESOURCE_STATES m_state = D3D12_RESOURCE_STATE_COMMON;
	D3D12_HEAP_TYPE m_heapType = D3D12_HEAP_TYPE_DEFAULT;
};

class DX12Texture : public ITexture
{
public:
	explicit DX12Texture(ComPtr<ID3D12Resource> res, bool isOwned);

	Format GetFormat() const override { return m_format; }
	uint32_t GetWidth()  const override { return m_width; }
	uint32_t GetHeight() const override { return m_height; }

	ID3D12Resource* GetNative() const { return m_resource.Get(); }
	D3D12_RESOURCE_STATES GetCurrentState() const { return m_state; }
	void SetCurrentState(D3D12_RESOURCE_STATES s) { m_state = s; }

private:
	ComPtr<ID3D12Resource> m_resource;
	bool m_owned = true;
	Format m_format = Format::Unknown;
	uint32_t m_width = 0, m_height = 0;
	D3D12_RESOURCE_STATES m_state = D3D12_RESOURCE_STATE_COMMON;
};

RHI_END
/////////////////////////////////////////////////////////////////
