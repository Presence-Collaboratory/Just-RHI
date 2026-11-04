/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#include "DX12Resources.h"
#include "DX12Conversions.h"
/////////////////////////////////////////////////////////////////
RHI_BEGIN

DX12Buffer::DX12Buffer(ID3D12Device* device, const BufferDesc& desc, D3D12_HEAP_TYPE heapType) : m_desc(desc), m_heapType(heapType)
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = heapType;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment        = 0;
	resDesc.Width            = desc.Size;
	resDesc.Height           = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels        = 1;
	resDesc.Format           = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags            = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES initState = (heapType == D3D12_HEAP_TYPE_UPLOAD) ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON;

	DX_CHECK(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, initState, nullptr, IID_PPV_ARGS(&m_resource)));

	m_state = initState;
	m_gpuAddress = m_resource->GetGPUVirtualAddress();
}

DX12Texture::DX12Texture(ComPtr<ID3D12Resource> res, bool isOwned) : m_resource(std::move(res)), m_owned(isOwned)
{
	if (m_resource)
	{
		D3D12_RESOURCE_DESC d = m_resource->GetDesc();
		m_format = Format::Unknown;
		m_width  = (uint32_t)d.Width;
		m_height = d.Height;
		m_state = D3D12_RESOURCE_STATE_PRESENT;
	}
}

RHI_END
/////////////////////////////////////////////////////////////////
