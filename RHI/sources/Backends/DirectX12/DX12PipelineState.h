/////////////////////////////////////////////////////////////////
// Just RHI - Simple minimalistic RHI
// Copyright (c) 2026 NSDeathman
// Licensed under the MIT License.
/////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////
#include "DX12Common.h"
#include <include/RHI_PipelineState.h>
/////////////////////////////////////////////////////////////////
RHI_BEGIN

class DX12PipelineState : public IPipelineState
{
public:
	DX12PipelineState(ID3D12Device* device, const PipelineStateDesc& desc);

	const PipelineStateDesc& GetDesc() const override { return m_desc; }
	void* GetNative() override { return m_pso.Get(); }
	ID3D12PipelineState* GetPSO() const { return m_pso.Get(); }
	ID3D12RootSignature* GetRootSignature() const { return m_rootSig.Get(); }

private:
	PipelineStateDesc m_desc;
	ComPtr<ID3D12RootSignature> m_rootSig;
	ComPtr<ID3D12PipelineState> m_pso;
};

RHI_END
/////////////////////////////////////////////////////////////////
