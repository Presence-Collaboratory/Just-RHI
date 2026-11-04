/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <RHI/include/RHI.h>
/////////////////////////////////////////////////////////////////
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
/////////////////////////////////////////////////////////////////
using namespace RHI;
/////////////////////////////////////////////////////////////////
static const char* kShaderSource = R"(
struct VSInput {
    float3 Position : POSITION;
    float3 Color    : COLOR;
};

struct PSInput {
    float4 Position : SV_POSITION;
    float3 Color    : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput o;
    o.Position = float4(input.Position, 1.0);
    o.Color    = input.Color;
    return o;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return float4(input.Color, 1.0);
}
)";

static bool g_running = true;

LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		g_running = false;
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if (w == VK_ESCAPE) { g_running = false; PostQuitMessage(0); }
		return 0;
	}
	return DefWindowProc(h, msg, w, l);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	WNDCLASSEXW wc = { sizeof(wc) };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInst;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"RHITriangleWindow";
	RegisterClassExW(&wc);

	RECT rc = { 0, 0, 1280, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindowExW(0, 
								wc.lpszClassName, 
								L"RHI Triangle",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, 
								CW_USEDEFAULT,
								rc.right - rc.left, 
								rc.bottom - rc.top,
								nullptr, 
								nullptr, 
								hInst, 
								nullptr);
	ShowWindow(hwnd, SW_SHOW);

	IDevice* device = CreateDevice(BackendType::DirectX12);
	if (!device)
	{
		MessageBoxW(hwnd, L"Failed to create device", L"Error", MB_ICONERROR);
		return 1;
	}

	DeviceDesc devDesc;
	devDesc.Width = 1280;
	devDesc.Height = 720;
	devDesc.FramesInFlight = 2;
	devDesc.BackBufferFormat = Format::R8G8B8A8_UNORM;
#ifdef _DEBUG
	devDesc.EnableDebugLayer = true;
#endif
	if (!device->Init(hwnd, devDesc))
	{
		MessageBoxW(hwnd, L"Failed to init device", L"Error", MB_ICONERROR);
		return 1;
	}

	ShaderCompileDesc vsDesc;
	vsDesc.Source = kShaderSource;
	vsDesc.SourceSize = strlen(kShaderSource);
	vsDesc.SourceName = "triangle.hlsl";
	vsDesc.EntryPoint = "VSMain";
	vsDesc.Target = "vs_5_1";
	auto vs = device->CompileShader(vsDesc);
	if (!vs.IsValid())
	{
		MessageBoxA(hwnd, vs.ErrorMessage.c_str(), "VS compile error", MB_ICONERROR);
		return 1;
	}

	ShaderCompileDesc psDesc;
	psDesc.Source = kShaderSource;
	psDesc.SourceSize = strlen(kShaderSource);
	psDesc.SourceName = "triangle.hlsl";
	psDesc.EntryPoint = "PSMain";
	psDesc.Target = "ps_5_1";
	auto ps = device->CompileShader(psDesc);
	if (!ps.IsValid())
	{
		MessageBoxA(hwnd, ps.ErrorMessage.c_str(), "PS compile error", MB_ICONERROR);
		return 1;
	}

	VertexElement elements[] = {
		{ "POSITION", 0, Format::R32G32B32_FLOAT, 0  },
		{ "COLOR",    0, Format::R32G32B32_FLOAT, 12 },
	};

	PipelineStateDesc psoDesc;
	psoDesc.VS_Bytecode = vs.Bytecode.data();
	psoDesc.VS_Size = vs.Bytecode.size();
	psoDesc.PS_Bytecode = ps.Bytecode.data();
	psoDesc.PS_Size = ps.Bytecode.size();
	psoDesc.VertexElements = elements;
	psoDesc.VertexElementCount = 2;
	psoDesc.VertexStride = 24;
	psoDesc.Topology = Topology::TriangleList;
	psoDesc.RTVFormat = Format::R8G8B8A8_UNORM;
	psoDesc.DSVFormat = Format::Unknown;
	psoDesc.DepthEnable = false;
	psoDesc.CullBackFace = false;
	psoDesc.DebugName = "TrianglePSO";
	IPipelineState* pso = device->CreatePipelineState(psoDesc);

	struct Vertex { float pos[3]; float col[3]; };
	Vertex vertices[] = {
		{ {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
	};
	BufferDesc vbDesc = BufferDesc::Vertex(sizeof(vertices), sizeof(Vertex));
	vbDesc.DebugName = "TriangleVB";
	IBuffer* vb = device->CreateBuffer(vbDesc, vertices);

	MSG msg = {};
	while (g_running)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!g_running) break;

		device->BeginFrame();

		auto* cmd = device->GetCommandList();
		auto* backbuffer = device->GetCurrentBackBuffer();
		auto  rtv = device->GetBackBufferRTV();

		cmd->Begin();

		cmd->Transition(backbuffer, ResourceState::RenderTarget);
		cmd->SetRenderTarget(rtv);
		cmd->ClearRenderTarget(rtv, { 0.1f, 0.1f, 0.1f, 1.0f });

		Viewport vp = { 0, 0, (float)devDesc.Width, (float)devDesc.Height, 0, 1 };
		cmd->SetViewport(vp);
		cmd->SetScissorRect({ 0, 0, (s32)devDesc.Width, (s32)devDesc.Height });

		cmd->SetPipelineState(pso);
		cmd->SetPrimitiveTopology(Topology::TriangleList);
		cmd->SetVertexBuffer(vb, 0);
		cmd->Draw(3);

		cmd->Transition(backbuffer, ResourceState::Present);
		cmd->End();

		ICommandList* lists[] = { cmd };
		device->ExecuteCommandLists(lists, 1);
		device->EndFrame();
	}

	device->WaitIdle();
	device->Destroy(vb);
	device->Destroy(pso);
	device->Shutdown();
	delete device;

	return 0;
}
/////////////////////////////////////////////////////////////////
