/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#include "DX12Device.h"
#include "DX12CommandList.h"
#include "DX12Conversions.h"
#include "DX12PipelineState.h"
#include "DX12Resources.h"
#include <d3dcompiler.h>
/////////////////////////////////////////////////////////////////
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
/////////////////////////////////////////////////////////////////
RHI_BEGIN

DX12Device::DX12Device() = default;

DX12Device::~DX12Device()
{
	Shutdown();
}

bool DX12Device::Init(HWND hwnd, const DeviceDesc & desc)
{
	m_desc = desc;
	m_hwnd = hwnd;

	if (desc.FramesInFlight < 2)
	{
		OutputDebugStringA("[DX12] FramesInFlight must be >= 2\n");
		return false;
	}

	if (desc.EnableDebugLayer)
	{
		ComPtr<ID3D12Debug> debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
			debug->EnableDebugLayer();
	}

	UINT factoryFlags = 0;
	if (desc.EnableDebugLayer)
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

	DX_CHECK_MSG(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory)), "CreateDXGIFactory2");

	ComPtr<IDXGIAdapter1> adapter;
	DXGI_ADAPTER_DESC1 adapterDesc = {};
	for (UINT i = 0; m_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		if (FAILED(adapter->GetDesc1(&adapterDesc))) continue;
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
			break;
		adapter.Reset();
	}

	if (!m_device)
	{
		ComPtr<IDXGIAdapter> warp;
		m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warp));
		DX_CHECK(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
	}

	D3D12_COMMAND_QUEUE_DESC qd = {};
	qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	qd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	DX_CHECK(m_device->CreateCommandQueue(&qd, IID_PPV_ARGS(&m_graphicsQueue)));

	if (!CreateSwapChain(hwnd)) return false;

	DX_CHECK(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence)));
	m_frameEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

	m_allocators.resize(desc.FramesInFlight);
	m_fenceValues.assign(desc.FramesInFlight, 0);
	for (uint32_t i = 0; i < desc.FramesInFlight; ++i)
		DX_CHECK(m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocators[i])));

	DX_CHECK(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_uploadFence)));
	m_uploadEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

	m_commandList = std::make_unique<DX12CommandList>(this, m_device.Get());

	if (!CreateRenderTargets()) return false;

	m_frameIndex = desc.FramesInFlight - 1;
	m_frameIndex = (m_frameIndex + 1) % m_desc.FramesInFlight;

	OutputDebugStringA("[DX12] Device initialised\n");
	return true;
}

bool DX12Device::CreateSwapChain(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 sd = {};
	sd.Width = m_desc.Width;
	sd.Height = m_desc.Height;
	sd.Format = ToDXGIFormat(m_desc.BackBufferFormat);
	sd.Stereo = FALSE;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = m_desc.FramesInFlight;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = 0;

	ComPtr<IDXGISwapChain1> swapChain1;
	DX_CHECK_MSG(m_factory->CreateSwapChainForHwnd(m_graphicsQueue.Get(), hwnd, &sd, nullptr, nullptr, &swapChain1), "CreateSwapChainForHwnd");

	DX_CHECK(swapChain1.As(&m_swapChain));

	m_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	return true;
}

bool DX12Device::CreateRenderTargets()
{
	D3D12_DESCRIPTOR_HEAP_DESC hd = {};
	hd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	hd.NumDescriptors = m_desc.FramesInFlight;
	hd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	DX_CHECK(m_device->CreateDescriptorHeap(&hd, IID_PPV_ARGS(&m_rtvHeap)));
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_backBufferRTVs.resize(m_desc.FramesInFlight);
	m_backBuffers.resize(m_desc.FramesInFlight);

	D3D12_CPU_DESCRIPTOR_HANDLE h = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (uint32_t i = 0; i < m_desc.FramesInFlight; ++i)
	{
		ComPtr<ID3D12Resource> res;
		DX_CHECK(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&res)));
		m_device->CreateRenderTargetView(res.Get(), nullptr, h);

		m_backBufferRTVs[i].Slot = i;
		m_backBuffers[i] = std::make_unique<DX12Texture>(res, false);

		h.ptr += m_rtvDescriptorSize;
	}

	return true;
}

bool DX12Device::BeginFrame()
{
	m_frameIndex = (m_frameIndex + 1) % m_desc.FramesInFlight;

	WaitForFrame(m_frameIndex);

	m_commandList->SetCurrentAllocator(m_allocators[m_frameIndex].Get());

	m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	return true;
}

void DX12Device::WaitForFrame(uint32_t frameIndex)
{
	if (m_frameFence->GetCompletedValue() < m_fenceValues[frameIndex])
	{
		DX_CHECK(m_frameFence->SetEventOnCompletion(m_fenceValues[frameIndex], m_frameEvent));
		WaitForSingleObject(m_frameEvent, INFINITE);
	}
}

void DX12Device::ExecuteCommandLists(ICommandList * *lists, uint32_t count)
{
	std::vector<ID3D12CommandList*> native(count);
	for (uint32_t i = 0; i < count; ++i)
		native[i] = static_cast<ID3D12CommandList*>(lists[i]->GetNative());

	m_graphicsQueue->ExecuteCommandLists(count, native.data());
}

void DX12Device::EndFrame()
{
	m_nextFenceValue++;
	DX_CHECK(m_graphicsQueue->Signal(m_frameFence.Get(), m_nextFenceValue));
	m_fenceValues[m_frameIndex] = m_nextFenceValue;

	DX_CHECK(m_swapChain->Present(1, 0));
}

void DX12Device::WaitIdle()
{
	m_nextFenceValue++;
	DX_CHECK(m_graphicsQueue->Signal(m_frameFence.Get(), m_nextFenceValue));
	DX_CHECK(m_frameFence->SetEventOnCompletion(m_nextFenceValue, m_frameEvent));
	WaitForSingleObject(m_frameEvent, INFINITE);
}

ICommandList* DX12Device::GetCommandList()
{ 
	return m_commandList.get(); 
}

IBuffer* DX12Device::CreateBuffer(const BufferDesc & desc, const void* initialData)
{
	D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
	if (desc.Usage & (BufferUsage_Dynamic | BufferUsage_Readback))
		heapType = (desc.Usage & BufferUsage_Readback) ? D3D12_HEAP_TYPE_READBACK : D3D12_HEAP_TYPE_UPLOAD;

	auto* buf = new DX12Buffer(m_device.Get(), desc, heapType);
	buf->SetDebugName(desc.DebugName ? desc.DebugName : "");

	if (initialData)
		UpdateBuffer(buf, initialData, (size_t)desc.Size, 0);

	return buf;
}

IPipelineState* DX12Device::CreatePipelineState(const PipelineStateDesc & desc)
{
	return new DX12PipelineState(m_device.Get(), desc);
}

void DX12Device::Destroy(IObject * obj)
{
	delete obj;
}

void DX12Device::UpdateBuffer(IBuffer * buf, const void* data,
	size_t size, size_t offset)
{
	auto* b = static_cast<DX12Buffer*>(buf);

	if (b->GetHeapType() == D3D12_HEAP_TYPE_UPLOAD)
	{
		void* mapped = nullptr;
		D3D12_RANGE readRange = { 0, 0 };
		DX_CHECK(b->GetNative()->Map(0, &readRange, &mapped));
		memcpy((uint8_t*)mapped + offset, data, size);
		D3D12_RANGE writeRange = { offset, offset + size };
		b->GetNative()->Unmap(0, &writeRange);
		return;
	}

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC stagingDesc = {};
	stagingDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	stagingDesc.Width = size;
	stagingDesc.Height = 1;
	stagingDesc.DepthOrArraySize = 1;
	stagingDesc.MipLevels = 1;
	stagingDesc.Format = DXGI_FORMAT_UNKNOWN;
	stagingDesc.SampleDesc.Count = 1;
	stagingDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> staging;
	DX_CHECK(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &stagingDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&staging)));

	void* mapped = nullptr;
	D3D12_RANGE readRange = { 0, 0 };
	DX_CHECK(staging->Map(0, &readRange, &mapped));
	memcpy(mapped, data, size);
	staging->Unmap(0, nullptr);

	ComPtr<ID3D12CommandAllocator> alloc;
	DX_CHECK(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc)));

	ComPtr<ID3D12GraphicsCommandList> cmd;
	DX_CHECK(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc.Get(), nullptr, IID_PPV_ARGS(&cmd)));

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = b->GetNative();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = b->GetCurrentState();
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	cmd->ResourceBarrier(1, &barrier);

	cmd->CopyBufferRegion(b->GetNative(), offset, staging.Get(), 0, size);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = b->GetCurrentState();
	cmd->ResourceBarrier(1, &barrier);

	DX_CHECK(cmd->Close());

	ID3D12CommandList* lists[] = { cmd.Get() };
	m_graphicsQueue->ExecuteCommandLists(1, lists);

	m_uploadFenceValue++;
	DX_CHECK(m_graphicsQueue->Signal(m_uploadFence.Get(), m_uploadFenceValue));
	if (m_uploadFence->GetCompletedValue() < m_uploadFenceValue)
	{
		DX_CHECK(m_uploadFence->SetEventOnCompletion(m_uploadFenceValue, m_uploadEvent));
		WaitForSingleObject(m_uploadEvent, INFINITE);
	}
}

ShaderCompileResult DX12Device::CompileShader(const ShaderCompileDesc & desc)
{
	ShaderCompileResult result;

	if (!desc.Source || desc.SourceSize == 0)
	{
		result.ErrorMessage = "Empty source";
		return result;
	}

	std::vector<D3D_SHADER_MACRO> macros;
	if (desc.Defines)
	{
		const char* const* p = desc.Defines;
		while (p[0] != nullptr)
		{
			macros.push_back({ p[0], p[1] });
			p += 2;
		}
		macros.push_back({ nullptr, nullptr });
	}

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ComPtr<ID3DBlob> bytecode, errors;
	HRESULT hr = D3DCompile(desc.Source, 
							desc.SourceSize,
							desc.SourceName, 
							macros.empty() ? nullptr : macros.data(),
							nullptr, 
							desc.EntryPoint, 
							desc.Target,
							flags, 
							0, 
							&bytecode, 
							&errors);

	if (FAILED(hr))
	{
		if (errors)
			result.ErrorMessage.assign(static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
		else
			result.ErrorMessage = "Unknown D3DCompile error";
		return result;
	}

	result.Bytecode.resize(bytecode->GetBufferSize());
	memcpy(result.Bytecode.data(), bytecode->GetBufferPointer(), bytecode->GetBufferSize());
	return result;
}

ITexture* DX12Device::GetCurrentBackBuffer()
{
	return m_backBuffers[m_currentBackBufferIndex].get();
}

RenderTargetView DX12Device::GetBackBufferRTV()
{
	return m_backBufferRTVs[m_currentBackBufferIndex];
}

DeviceStatus DX12Device::CheckDeviceStatus() const
{
	if (!m_device) return DeviceStatus::Lost;
	HRESULT hr = m_device->GetDeviceRemovedReason();
	return SUCCEEDED(hr) ? DeviceStatus::OK : DeviceStatus::Lost;
}

void DX12Device::Shutdown()
{
	if (m_device)
		WaitIdle();

	m_commandList.reset();
	m_backBuffers.clear();
	m_backBufferRTVs.clear();
	m_allocators.clear();
	m_rtvHeap.Reset();
	m_swapChain.Reset();
	m_frameFence.Reset();
	m_uploadFence.Reset();
	m_graphicsQueue.Reset();
	m_device.Reset();
	m_factory.Reset();

	if (m_frameEvent) { CloseHandle(m_frameEvent);  m_frameEvent = nullptr; }
	if (m_uploadEvent) { CloseHandle(m_uploadEvent); m_uploadEvent = nullptr; }
}

RHI_END
/////////////////////////////////////////////////////////////////
