/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "DX12Common.h"
#include <include/RHI_Device.h>
#include <vector>
#include <string>
#include <memory>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

class DX12CommandList;

class DX12Device : public IDevice
{
public:
	DX12Device();
	~DX12Device() override;

	bool Init(HWND hwnd, const DeviceDesc& desc) override;
	void Shutdown() override;
	void WaitIdle() override;

	bool BeginFrame() override;
	void ExecuteCommandLists(ICommandList** lists, uint32_t count) override;
	void EndFrame() override;

	ICommandList* GetCommandList() override;

	IBuffer* CreateBuffer(const BufferDesc& desc, const void* initialData = nullptr) override;
	IPipelineState* CreatePipelineState(const PipelineStateDesc& desc) override;
	void Destroy(IObject* obj) override;

	void UpdateBuffer(IBuffer* buf, const void* data, size_t size, size_t offset = 0) override;

	ShaderCompileResult CompileShader(const ShaderCompileDesc& desc) override;

	ITexture* GetCurrentBackBuffer() override;
	RenderTargetView GetBackBufferRTV()    override;
	uint32_t GetWidth()  const override { return m_desc.Width; }
	uint32_t GetHeight() const override { return m_desc.Height; }
	Format GetBackBufferFormat() const override { return m_desc.BackBufferFormat; }

	DeviceStatus CheckDeviceStatus() const override;
	BackendType GetBackendType() const override { return BackendType::DirectX12; }

	void* GetNativeDevice() override { return m_device.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t slot) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE h = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		h.ptr += (SIZE_T)slot * m_rtvDescriptorSize;
		return h;
	}

	ID3D12Device* GetDevice() const { return m_device.Get(); }
	ID3D12CommandQueue* GetGraphicsQueue() const { return m_graphicsQueue.Get(); }

private:
	bool CreateSwapChain(HWND hwnd);
	bool CreateRenderTargets();
	void WaitForFrame(uint32_t frameIndex);
	void FlushUpload();

	DeviceDesc m_desc{};
	HWND m_hwnd = nullptr;

	ComPtr<IDXGIFactory6>       m_factory;
	ComPtr<ID3D12Device>        m_device;
	ComPtr<ID3D12CommandQueue>  m_graphicsQueue;
	ComPtr<IDXGISwapChain4>     m_swapChain;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	uint32_t m_rtvDescriptorSize = 0;

	std::vector<RenderTargetView> m_backBufferRTVs;

	std::vector<std::unique_ptr<class DX12Texture>> m_backBuffers;
	uint32_t m_currentBackBufferIndex = 0;

	std::vector<ComPtr<ID3D12CommandAllocator>> m_allocators;
	std::vector<uint64_t> m_fenceValues;
	ComPtr<ID3D12Fence> m_frameFence;
	HANDLE m_frameEvent = nullptr;
	uint64_t m_nextFenceValue = 1;
	uint32_t m_frameIndex = 0;

	std::unique_ptr<DX12CommandList> m_commandList;

	ComPtr<ID3D12Fence> m_uploadFence;
	HANDLE m_uploadEvent = nullptr;
	uint64_t m_uploadFenceValue = 0;
};

RHI_END
/////////////////////////////////////////////////////////////////
