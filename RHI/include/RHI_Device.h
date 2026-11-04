/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "RHI_API.h"
#include "RHI_Macros.h"
#include "RHI_Types.h"
#include "RHI_Resources.h"
#include "RHI_PipelineState.h"
#include "RHI_CommandList.h"
#include <string>
#include <vector>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

struct DeviceDesc
{
	uint32_t Width = 1280;
	uint32_t Height = 720;
	uint32_t FramesInFlight = 2;

	Format BackBufferFormat = Format::R8G8B8A8_UNORM;
	Format DepthStencilFormat = Format::Unknown;

	bool EnableDebugLayer = false;
	bool EnableGPUValidation = false;

	const char* DebugName = "XRHI Device";
};

// -----------------------------------------------------------------------------
// Компиляция шейдеров
// -----------------------------------------------------------------------------
struct ShaderCompileDesc
{
	const char* Source = nullptr;
	size_t      SourceSize = 0;
	const char* SourceName = "<memory>";
	const char* EntryPoint = "main";
	const char* Target = "vs_5_1";

	const char* const* Defines = nullptr;

	uint32_t Flags = 0;
};

struct ShaderCompileResult
{
	std::vector<uint8_t> Bytecode;
	std::string ErrorMessage;

	bool IsValid() const { return !Bytecode.empty(); }
};

class IDevice
{
public:
	virtual ~IDevice() = default;

	virtual bool Init(HWND hwnd, const DeviceDesc& desc) = 0;
	virtual void Shutdown() = 0;
	virtual void WaitIdle() = 0;

	virtual bool BeginFrame() = 0;
	virtual void ExecuteCommandLists(ICommandList** lists, uint32_t count) = 0;
	virtual void EndFrame() = 0;

	virtual ICommandList* GetCommandList() = 0;

	virtual IBuffer* CreateBuffer(const BufferDesc& desc, const void* initialData = nullptr) = 0;
	virtual IPipelineState* CreatePipelineState(const PipelineStateDesc& desc) = 0;
	virtual void Destroy(IObject* obj) = 0;

	virtual void UpdateBuffer(IBuffer* buf, const void* data, size_t size, size_t offset = 0) = 0;

	virtual ShaderCompileResult CompileShader(const ShaderCompileDesc& desc) = 0;

	virtual ITexture* GetCurrentBackBuffer() = 0;
	virtual RenderTargetView GetBackBufferRTV() = 0;
	virtual uint32_t GetWidth()  const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual Format GetBackBufferFormat() const = 0;

	virtual DeviceStatus CheckDeviceStatus() const = 0;
	virtual BackendType GetBackendType() const = 0;

	virtual void* GetNativeDevice() = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
	RHI_API IDevice* CreateDevice(BackendType type);
#ifdef __cplusplus
}
#endif

RHI_END
/////////////////////////////////////////////////////////////////
