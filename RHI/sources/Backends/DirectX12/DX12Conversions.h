/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "DX12Common.h"
#include <include/RHI_Types.h>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

inline DXGI_FORMAT ToDXGIFormat(Format f)
{
	switch (f)
	{
	case Format::R8G8B8A8_UNORM:       return DXGI_FORMAT_R8G8B8A8_UNORM;
	case Format::R8G8B8A8_UNORM_SRGB:  return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case Format::B8G8R8A8_UNORM:       return DXGI_FORMAT_B8G8R8A8_UNORM;
	case Format::B8G8R8A8_UNORM_SRGB:  return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case Format::R16G16_FLOAT:         return DXGI_FORMAT_R16G16_FLOAT;
	case Format::R32G32_FLOAT:         return DXGI_FORMAT_R32G32_FLOAT;
	case Format::R32G32B32_FLOAT:      return DXGI_FORMAT_R32G32B32_FLOAT;
	case Format::R32G32B32A32_FLOAT:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case Format::D24_UNORM_S8_UINT:    return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case Format::D32_FLOAT:            return DXGI_FORMAT_D32_FLOAT;
	default:                           return DXGI_FORMAT_UNKNOWN;
	}
}

inline D3D12_PRIMITIVE_TOPOLOGY ToD3D12Topology(Topology t)
{
	switch (t)
	{
	case Topology::PointList:     return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case Topology::LineList:      return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case Topology::LineStrip:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case Topology::TriangleList:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case Topology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:                      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

inline D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12TopologyType(Topology t)
{
	switch (t)
	{
	case Topology::PointList:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case Topology::LineList:
	case Topology::LineStrip:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case Topology::TriangleList:
	case Topology::TriangleStrip:  return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	default:                       return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
}

inline D3D12_RESOURCE_STATES ToD3D12State(ResourceState s)
{
	switch (s)
	{
	case ResourceState::Common:        return D3D12_RESOURCE_STATE_COMMON;
	case ResourceState::VertexBuffer:  return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	case ResourceState::IndexBuffer:   return D3D12_RESOURCE_STATE_INDEX_BUFFER;
	case ResourceState::ConstantBuffer:return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	case ResourceState::ShaderResource:return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	case ResourceState::UnorderedAccess:return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	case ResourceState::RenderTarget:  return D3D12_RESOURCE_STATE_RENDER_TARGET;
	case ResourceState::DepthRead:     return D3D12_RESOURCE_STATE_DEPTH_READ;
	case ResourceState::DepthWrite:    return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	case ResourceState::CopySource:    return D3D12_RESOURCE_STATE_COPY_SOURCE;
	case ResourceState::CopyDest:      return D3D12_RESOURCE_STATE_COPY_DEST;
	case ResourceState::Present:       return D3D12_RESOURCE_STATE_PRESENT;
	default:                           return D3D12_RESOURCE_STATE_COMMON;
	}
}

RHI_END
/////////////////////////////////////////////////////////////////
