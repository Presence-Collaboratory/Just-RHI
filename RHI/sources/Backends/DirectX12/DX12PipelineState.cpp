/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#include "DX12PipelineState.h"
#include "DX12Conversions.h"
/////////////////////////////////////////////////////////////////
RHI_BEGIN

DX12PipelineState::DX12PipelineState(ID3D12Device* device, const PipelineStateDesc& desc) : m_desc(desc)
{
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 0;
	rsDesc.pParameters = nullptr;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.pStaticSamplers = nullptr;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> sigBlob, errBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	if(FAILED(hr))
	{
		if(errBlob)
			OutputDebugStringA((const char*)errBlob->GetBufferPointer());
		DX_CHECK_MSG(hr, "D3D12SerializeRootSignature");
	}

	DX_CHECK(device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSig)));

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElems;
	inputElems.reserve(desc.VertexElementCount);
	for(uint32_t i = 0; i < desc.VertexElementCount; ++i)
	{
		const VertexElement& e = desc.VertexElements[i];
		D3D12_INPUT_ELEMENT_DESC d = {};
		d.SemanticName = e.SemanticName;
		d.SemanticIndex = e.SemanticIndex;
		d.Format = ToDXGIFormat(e.Format);
		d.InputSlot = e.InputSlot;
		d.AlignedByteOffset = e.Offset;
		d.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		d.InstanceDataStepRate = 0;
		inputElems.push_back(d);
	}

	D3D12_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = FALSE;
	blend.IndependentBlendEnable = FALSE;
	for(int i = 0; i < 8; ++i)
	{
		blend.RenderTarget[i].BlendEnable = FALSE;
		blend.RenderTarget[i].LogicOpEnable = FALSE;
		blend.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
		blend.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
		blend.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
		blend.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
		blend.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
		blend.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blend.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
		blend.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	D3D12_RASTERIZER_DESC raster = {};
	raster.FillMode = desc.Wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	raster.CullMode = desc.CullBackFace ? D3D12_CULL_MODE_BACK : D3D12_CULL_MODE_NONE;
	raster.FrontCounterClockwise = FALSE;
	raster.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	raster.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	raster.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	raster.DepthClipEnable = TRUE;
	raster.MultisampleEnable = FALSE;
	raster.AntialiasedLineEnable = FALSE;
	raster.ForcedSampleCount = 0;
	raster.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_DEPTH_STENCIL_DESC depth = {};
	depth.DepthEnable = desc.DepthEnable ? TRUE : FALSE;
	depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depth.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depth.StencilEnable = FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = m_rootSig.Get();
	pso.VS = {desc.VS_Bytecode, desc.VS_Size};
	pso.PS = {desc.PS_Bytecode, desc.PS_Size};
	pso.BlendState = blend;
	pso.SampleMask = UINT_MAX;
	pso.RasterizerState = raster;
	pso.DepthStencilState = depth;
	pso.InputLayout = {inputElems.data(), (UINT)inputElems.size()};
	pso.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = ToDXGIFormat(desc.RTVFormat);
	pso.DSVFormat = ToDXGIFormat(desc.DSVFormat);
	pso.SampleDesc = {1, 0};
	pso.NodeMask = 0;
	pso.CachedPSO = {nullptr, 0};
	pso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	DX_CHECK_MSG(device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&m_pso)), "CreateGraphicsPipelineState");
}

RHI_END
/////////////////////////////////////////////////////////////////
