#include "EnginePCH.h"
#include "RenderCommands.h"
#include "Renderer.h"



static void(__cdecl ID3D11DeviceContext::* SetSampler[EShaderStage::COUNT])
(UINT StartSlot, UINT NumViews, ID3D11SamplerState* const* ppShaderResourceViews) =
{
	&ID3D11DeviceContext::VSSetSamplers,
	&ID3D11DeviceContext::GSSetSamplers,
	&ID3D11DeviceContext::DSSetSamplers,
	&ID3D11DeviceContext::HSSetSamplers,
	&ID3D11DeviceContext::PSSetSamplers,
	&ID3D11DeviceContext::CSSetSamplers,
};





SClearCommand SClearCommand::Depth(float depthClearValue)
{
	const bool bDoClearColor = false;
	const bool bDoClearDepth = true;
	const bool bDoClearStencil = false;
	return SClearCommand(
		bDoClearColor, bDoClearDepth, bDoClearStencil,
		{ 0, 0, 0, 0 }, depthClearValue, 0
	);
}

SClearCommand SClearCommand::Color(const SVector4& InClearColor)
{
	const bool bDoClearColor = true;
	const bool bDoClearDepth = false;
	const bool bDoClearStencil = false;
	return SClearCommand(
		bDoClearColor, bDoClearDepth, bDoClearStencil,
		InClearColor, 0, 0
	);
}

SClearCommand SClearCommand::Color(float r, float g, float b, float a)
{
	return SClearCommand::Color(SVector4(r,g,b,a));
}

/*https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetshaderresources */

void SSetTextureCommand::SetResource(Renderer* pRenderer)
{
	ID3D11DeviceContext* pContext = pRenderer->GetDeviceContext();
	ID3D11Device* pDevice = pRenderer->GetDeivce();
	if (bUnorderedAccess)
	{
		constexpr UINT NUM_UAVs = 1;

		// UAV
		//
		if (numTextures == 1)
		{
			const FTextureID textureID = TextureIDs[0];
			assert(textureID >= 0);
			//pRenderer->m_Textures[textureID]._uavArray.size() != 0;
			const bool bUseArray = pRenderer->GetTextureObject(textureID)->m_UAVArray.size() != 0;				
			ID3D11UnorderedAccessView** ppUAVs = !bUseArray
				? &(pRenderer->GetTextureObject(textureID)->m_UAV)
				: &(pRenderer->GetTextureObject(textureID)->m_UAVArray[slice]);

			pContext->CSSetUnorderedAccessViews(Binding.TextureSlot, NUM_UAVs, ppUAVs, nullptr);
		}

		// UAV ARRAY
		//
		else
		{
			//assert(false);
			//Make UAV vector..
			std::vector< ID3D11UnorderedAccessView* > pUAVs(numTextures);
			for (uint32 i = 0; i < numTextures; ++i)
			{
				const FTextureID textureID = TextureIDs[i];
				assert(textureID >= 0);
				pUAVs[i] = pRenderer->GetTextureObject(textureID)->m_UAV;
			}
			pContext->CSSetUnorderedAccessViews(Binding.TextureSlot, numTextures, pUAVs.data(), nullptr);
		}
	}
	else
	{
		constexpr UINT NUM_SRVs = 1;

		// SRV
		//
		if (numTextures == 1)
		{
			const FTextureID textureID = TextureIDs[0];
			assert(textureID >= 0);
			const bool bUseArray = pRenderer->GetTextureObject(textureID)->m_SRVArray.size() != 0;				
			ID3D11ShaderResourceView** ppSRVs = !bUseArray
				? &pRenderer->GetTextureObject(textureID)->m_SRV
				: &pRenderer->GetTextureObject(textureID)->m_SRVArray[slice];
			(pContext->*SetShaderResources[Binding.ShaderStage])(Binding.TextureSlot, NUM_SRVs, ppSRVs);
		}

		// SRV ARRAY
		//
		else
		{
			//assert(false);
			std::vector<ID3D11ShaderResourceView* > pSRVs(numTextures);
			for (uint32 i = 0; i < numTextures; ++i)
			{
				const FTextureID textureID = TextureIDs[i];
				assert(textureID >= 0);
				pSRVs[i] = pRenderer->GetTextureObject(textureID)->m_SRV;					
			}

			(pContext->*SetShaderResources[Binding.ShaderStage])(Binding.TextureSlot, numTextures, pSRVs.data());
		}
	}
}

void SSetSamplerCommand::SetResource(Renderer* pRenderer)
{
	assert(SamplerID >= 0);
	(pRenderer->GetDeviceContext()->*SetSampler[Binding.ShaderStage])(Binding.SamplerSlot, 1, &pRenderer->GetSampler(SamplerID)->m_SamplerState);
}

void SSetRWBufferCommand::SetResource(Renderer* pRenderer)
{	
	assert(BufferID >= 0);
	ID3D11ShaderResourceView* srv = pRenderer->GetRWStructuredBuffer(BufferID).Srv;
	ID3D11UnorderedAccessView* uav = pRenderer->GetRWStructuredBuffer(BufferID).Uav;
	

	

	if(Binding.ShaderStage == EShaderStage::CS)
	{
		uint32 Count = -1;
		pRenderer->GetDeviceContext()->CSSetUnorderedAccessViews(Binding.RWSlot, 1, &uav, &Count);
	}
	else
	{
		(pRenderer->GetDeviceContext()->*SetShaderResources[Binding.ShaderStage])(Binding.RWSlot, 1, &srv);
	}
	

	
}

//void SDrawLineCommand::SetConstants(class Renderer* pRenderer)
//{
//	pRenderer->SetConstant3f("p1", p1);
//	pRenderer->SetConstant3f("p2", p2);
//	pRenderer->SetConstant3f("color", color.ToVector3());
//	pRenderer->Apply();
//	pRenderer->DrawIndexed(1, EPrimitiveTopology::POINT_LIST);
//
//}

void SDrawLineCommand::SetConstants(std::vector<SlineInfo>& lines, int32 idx)
{
	lines[idx].p1 = p1;
	lines[idx].p2 = p2;
	lines[idx].color= color.ToVector3();
}


void SDrawSolidCommand::SetConstants(std::vector<SSolidInfo>& solids, int32 idx)
{
	solids[idx].p1 = p1;
	solids[idx].p2 = p2;
	solids[idx].p3 = p3;
	solids[idx].color = color.ToVector3();
}
