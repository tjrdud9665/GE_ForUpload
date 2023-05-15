#include "EnginePCH.h"
#include "RenderPass.h"
//#include "Shader.h"
#include "Renderer.h"
#include "GameFramework/Components/SceneComponent.h"
#include "GameFramework/Components/LightComponent.h"
#include "RenderManager.h"
#include "Assets/Mesh/MeshAsset.h"
#include <Assets/Mesh/SkeletalMeshAsset.h>



void SDeferredRenderingPasses::Initialize(class Renderer* pRenderer, bool bAAResolve, const DXGI_FORMAT& targetFormet)
{
	if(!ShaderInitilized)
	{
	

	const char* pFSQ_VS = "FullScreenQuad_vs.hlsl";
	//const char* pLight_VS = "MVPTransformationWithUVs_vs.hlsl";

	//version Not Instanced
	const SShaderDesc geomShaderDesc = 
	{	"GBufferPass", 
		{
		SShaderStageDesc{ "Deferred_Geometry_vs.hlsl", {} },
		SShaderStageDesc{ "Deferred_Geometry_ps.hlsl", {} }		
		} 
	};



	//Version Instanced
	const std::vector<SShaderMacro> instancedGeomShaderMacros =
	{
		SShaderMacro{ "INSTANCED", "1" }
		
	};
	const SShaderDesc geomShaderInstancedDesc = { "InstancedGBufferPass",
	{
		SShaderStageDesc{ "Deferred_Geometry_vs.hlsl", instancedGeomShaderMacros },
		SShaderStageDesc{ "Deferred_Geometry_ps.hlsl", instancedGeomShaderMacros }
	} };


	const SShaderDesc ambientShaderDesc = { "Deferred_Ambient",
	{
		SShaderStageDesc{ pFSQ_VS, {} },
		SShaderStageDesc{ "deferred_brdf_ambient_ps.hlsl", {} }
	} };
	const SShaderDesc ambientIBLShaderDesc = { "Deferred_AmbientIBL",
	{
		SShaderStageDesc{ pFSQ_VS, {} },
		SShaderStageDesc{ "deferred_brdf_ambientIBL_ps.hlsl", {} }
	} };
	const SShaderDesc BRDFLightingShaderDesc = { "Deferred_BRDF_Lighting",
	{
		SShaderStageDesc{ pFSQ_VS, {} },
		SShaderStageDesc{ "deferred_brdf_lighting_ps.hlsl", {} }
	} };


	SShaderDesc tonemappingShaderDesc = SShaderDesc{ "Tonemapping_HDR",
	SShaderStageDesc{ pFSQ_VS              , {} },
	SShaderStageDesc{ "Tonemapping_ps.hlsl", { {"HDR_TONEMAPPER", "1"} } }
	};
	_toneMappingShaderHDR = pRenderer->CreateShader(tonemappingShaderDesc);
	//NoInstancingversion
	GeometryShader = pRenderer->CreateShader(geomShaderDesc);
	//Ins Version
	GeometryInstancedShader = pRenderer->CreateShader(geomShaderInstancedDesc);

	AmbientShader = pRenderer->CreateShader(ambientShaderDesc);
	AmbientIBLShader = pRenderer->CreateShader(ambientIBLShaderDesc);

	BRDFLightingShader = pRenderer->CreateShader(BRDFLightingShaderDesc);

	D3D11_SAMPLER_DESC blurSamplerDesc = {};
	blurSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	blurSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	blurSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	blurSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	blurSmp = pRenderer->CreateSamplerState(blurSamplerDesc, "blurSmp");

	ShaderInitilized = true;
	}

	//CreateRenderTaget
	InitializeGBuffer(pRenderer);

	const DXGI_FORMAT format = targetFormet;
	SRenderTargetDesc rtDesc = {};
	rtDesc.TextureDesc.Width = pRenderer->GetAASetting().ResolutionX; // TODO: what if aa turned off?
	rtDesc.TextureDesc.Height = pRenderer->GetAASetting().ResolutionY; // TODO: what if aa turned off?
	rtDesc.TextureDesc.mipCount = 1;
	rtDesc.TextureDesc.arraySize = 1;
	rtDesc.TextureDesc.Format = format;
	rtDesc.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
	rtDesc.Format = format;
	ShaderRenderTarget = pRenderer->AddRenderTarget(rtDesc);
	AmbientRenderTarget = pRenderer->AddRenderTarget(rtDesc);


}

/*
struct PSOut	// G-BUFFER
{
	float4 diffuseRoughness : SV_TARGET0;
	float4 specularMetalness : SV_TARGET1;
	float4 normals : SV_TARGET2;
	float3 emissiveColor : SV_TARGET3;
};
*/

void SDeferredRenderingPasses::InitializeGBuffer(class Renderer* pRenderer)
{
	SRenderTargetDesc rtDesc_f4;
	SRenderTargetDesc rtDesc_f3;
	//DiffuseTarget

	rtDesc_f4.TextureDesc.Width = pRenderer->GetAASetting().ResolutionX;
	rtDesc_f4.TextureDesc.Height = pRenderer->GetAASetting().ResolutionY;
	rtDesc_f4.TextureDesc.mipCount = 1;
	rtDesc_f4.TextureDesc.arraySize = 1;
	rtDesc_f4.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
	rtDesc_f4.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtDesc_f4.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	//
	//rtDesc_f3 = rtDesc_f4;

	//rtDesc_f3.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//rtDesc_f3.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;



	GBuffer.RTNormals = pRenderer->AddRenderTarget(rtDesc_f4);
	GBuffer.RTDiffuseRoughness = pRenderer->AddRenderTarget(rtDesc_f4);
	GBuffer.RTSpecularMetallic = pRenderer->AddRenderTarget(rtDesc_f4);
	GBuffer.RTEmissive = pRenderer->AddRenderTarget(rtDesc_f4);
	GBuffer.Initialized = true;


	// http://download.nvidia.com/developer/presentations/2004/6800_Leagues/6800_Leagues_Deferred_Shading.pdf
	// Option: trade storage for computation
	//  - Store pos.z     and compute xy from z + window.xy		(implemented)
	//	- Store normal.xy and compute z = sqrt(1 - x^2 - y^2)
	//
	{	// Geometry depth stencil state descriptor
		D3D11_DEPTH_STENCILOP_DESC dsOpDesc = {};
		dsOpDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsOpDesc.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
		dsOpDesc.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
		dsOpDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;

		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.FrontFace = dsOpDesc;
		desc.BackFace = dsOpDesc;

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		desc.StencilEnable = true;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;

		GeometryStencilState = pRenderer->AddDepthStencilState(desc);

		desc.StencilEnable = false;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_EQUAL;
		GeometryStencilStatePreZ = pRenderer->AddDepthStencilState(desc);
	}

}

void SDeferredRenderingPasses::ClearGBuffer(class Renderer* pRenderer)
{
	const bool bDoClearColor = true;
	const bool bDoClearDepth = false;
	const bool bDoClearStencil = false;
	SClearCommand clearCmd(
		bDoClearColor, bDoClearDepth, bDoClearStencil,
		{ 0, 0, 0, 0 }, 0, 0
	);
	pRenderer->BindRenderTargets(GBuffer.RTDiffuseRoughness, GBuffer.RTSpecularMetallic, GBuffer.RTNormals);
	pRenderer->BeginRender(clearCmd);
}

void SDeferredRenderingPasses::RenderGBuffer(class Renderer* pRenderer, const SSceneView& sceneView) const
{

	bool bDoClearColor = true;
	bool bDoClearDepth = true;
	bool bDoClearStencil = true;
	SClearCommand clearCmd(
		bDoClearColor, bDoClearDepth, bDoClearStencil,
		SVector4(0.0f), 1, 0
	);	

	pRenderer->SetShader(GeometryShader);		
	pRenderer->SetViewport(pRenderer->GetAASetting().ResolutionX, pRenderer->GetAASetting().ResolutionY);
	pRenderer->BindRenderTargets(GBuffer.RTDiffuseRoughness, GBuffer.RTSpecularMetallic, GBuffer.RTNormals, GBuffer.RTEmissive);
	
	
	pRenderer->BindDepthTarget(pRenderer->GetDefaultDepthTarget());
	pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_BACK);

	//pRenderer->SetDepthStencilState(UseDepthPrepass ? GeometryStencilStatePreZ : GeometryStencilState);
	pRenderer->SetDepthStencilState(GeometryStencilState);		
	pRenderer->SetSamplerState("sNormalSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
	pRenderer->SetSamplerState("sAnisoSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
	pRenderer->BeginRender(clearCmd);
	

	//OpaqueList중에서  같은 Mesh 를 사용한다면
	//인스턴싱으로 처리한다
	{
		FRenderingList::const_iterator iter = sceneView.OpaqueList.begin();
		FRenderingList::const_iterator iterEnd = sceneView.OpaqueList.end();

		for (; iter != iterEnd; iter++)
		{
			(*iter)->Render(pRenderer);
		}
	}


		
	if(sceneView.InstanceList.size()!=0)
	{
		pRenderer->SetShader(GeometryInstancedShader);

		pRenderer->BindRenderTargets(GBuffer.RTDiffuseRoughness, GBuffer.RTSpecularMetallic, GBuffer.RTNormals, GBuffer.RTEmissive);
		if (!UseDepthPrepass)
			pRenderer->BindDepthTarget(pRenderer->GetDefaultDepthTarget());

		//pRenderer->SetDepthStencilState(GeometryStencilState);

		pRenderer->SetSamplerState("sNormalSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
		pRenderer->SetSamplerState("sAnisoSampler", EDefaultSamplerState::ANISOTROPIC_4_WRAPPED_SAMPLER);
	
	{
		FInstancingRenderList::const_iterator iter = sceneView.InstanceList.begin();
		FInstancingRenderList::const_iterator iterEnd = sceneView.InstanceList.end();
		
		for (; iter != iterEnd; iter++)
		{			
			//Gathering Instanced Rendering Contexts~
			//Bind IA Pair 
			FRenderingList::const_iterator iter_perMesh = (*iter).second.begin();
			FRenderingList::const_iterator iter_perMesh_end = (*iter).second.end();

			std::vector<SInstanceInfo> instanceInfo;
			instanceInfo.resize((*iter).second.size());
			int32 InstanceID = 0;

			for (; iter_perMesh != iter_perMesh_end; iter_perMesh++, InstanceID++)
			{
				(*iter_perMesh)->GatherRenderInstance(pRenderer, instanceInfo, InstanceID);
			}				

			int32 MeshCount = (*iter).first->GetMeshCount();

			for (int32 i = 0; i < MeshCount; i++)
			{
				size_t pairCount = (*iter).first->GetMesh(i).GetIAPairSize();

				for (size_t a = 0; a < pairCount; a++)
				{
					FIAPair iaPair = (*iter).first->GetMesh(i).GetIAPair((int32)a);
					pRenderer->SetVertexBuffer(iaPair.first);
					pRenderer->SetIndexBuffer(iaPair.second);	

					std::vector<SObjectMatrices> matrices;
					std::vector<SSurfaceMaterial> surfaces;
					std::vector<int32>	useSkinning;
					std::vector<SVector3>	rootOffset;
					matrices.resize(InstanceID);
					surfaces.resize(InstanceID);
					useSkinning.resize(InstanceID);
					rootOffset.resize(InstanceID);

					for (int32 i = 0; i < InstanceID; i++)
					{
						matrices[i] = instanceInfo[i].objMatrics;
						surfaces[i] = instanceInfo[i].objSurface;
						useSkinning[i] = instanceInfo[i].useSkinning;
						rootOffset[i] = instanceInfo[i].rootOffset;
					}
					(*iter).first->GetMaterial((int32)a)->SetMaterialConstants(pRenderer, EShaders::FORWARD_PHONG);

					if ((*iter).first->GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
					{
						SkeletalMeshAsset* skeletalMesh = (SkeletalMeshAsset*)((*iter).first);
						if (skeletalMesh->GetInstancingBoneMatrixArray() != INDEX_NONE)
							pRenderer->SetStructured("InstancingComputedMatrix", skeletalMesh->GetInstancingBoneMatrixArray());						
					}


					
					pRenderer->SetStructured("InstancedObjMatrices", matrices.data(), sizeof(SObjectMatrices) * instanceInfo.size());
					pRenderer->SetStructured("InstancedSurfaceMaterial", surfaces.data(), sizeof(SSurfaceMaterial) * instanceInfo.size());
					pRenderer->SetStructured("InstancedUseSkinning", useSkinning.data(), sizeof(int32) * instanceInfo.size());
					pRenderer->SetStructured("InstancedRootOffset", rootOffset.data(), sizeof(SVector3) * instanceInfo.size());

					pRenderer->Apply();					
					pRenderer->DrawIndexedInstanced(InstanceID);

				}
			}
			
		
		}
	}

	}

	//Gathering LightingData	


}

void SDeferredRenderingPasses::RenderLightingPass(class Renderer* pRenderer, class RenderManager* pRenderManager, const SSceneView& sceneView, const SFrameLightingData& data) const
{
	pRenderer->UnbindDepthTarget();
	pRenderer->UnbindRenderTargets();	
	pRenderer->Apply();	
	
	pRenderer->SetViewport(pRenderer->GetAASetting().ResolutionX, pRenderer->GetAASetting().ResolutionY);

	pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_BACK);
	pRenderer->SetBlendState(EDefaultBlendState::ADDITIVE_COLOR	);
	pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED);
	pRenderer->BindRenderTarget(AmbientRenderTarget);
	
	
	pRenderer->BeginRender(SClearCommand::Color(SVector4(0, 0, 0, 0.0f)));

	FTextureID texIrradianceMap = sceneView.environmentMap.irradianceMap;
	FTextureID texSpecularMap = sceneView.environmentMap.prefilteredEnvironmentMap;
	FTextureID tBRDFLUT = EnvironmentMap::sBRDFIntegrationLUTTexture;	

	

	// AMBIENT LIGHTING
	//-----------------------------------------------------------------------------------------
	const bool bSkylight = texIrradianceMap != -1;	
	if(sceneView.environmentMap.envMapSampler != INDEX_NONE && bSkylight)
	{
		pRenderer->SetShader(AmbientIBLShader, false, false);
		pRenderer->SetTexture("tDiffuseRoughnessMap", pRenderer->GetRenderTargetTexture(GBuffer.RTDiffuseRoughness));
		pRenderer->SetTexture("tSpecularMetalnessMap", pRenderer->GetRenderTargetTexture(GBuffer.RTSpecularMetallic));
		pRenderer->SetTexture("tNormalMap", pRenderer->GetRenderTargetTexture(GBuffer.RTNormals));
		pRenderer->SetTexture("tDepthMap", pRenderer->GetDefaultDepthTargetTexture());
		pRenderer->SetTexture("tAmbientOcclusion", sceneView.AOTex);	// SSAO Tex

		pRenderer->SetTexture("tIrradianceMap", texIrradianceMap);

		pRenderer->SetTexture("tPreFilteredEnvironmentMap", texSpecularMap);

		pRenderer->SetTexture("tBRDFIntegrationLUT", tBRDFLUT);
		pRenderer->SetSamplerState("sEnvMapSampler", sceneView.environmentMap.envMapSampler);
		pRenderer->SetSamplerState("sWrapSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
		pRenderer->SetConstant4x4f("matViewInverse", sceneView.matViewInverse);
		pRenderer->SetConstant4x4f("matProjInverse", sceneView.matProjInverse);
	} else
	{
		
		pRenderer->SetShader(AmbientShader);
		pRenderer->SetTexture("tDiffuseRoughnessMap", pRenderer->GetRenderTargetTexture(GBuffer.RTDiffuseRoughness));
		pRenderer->SetTexture("tAmbientOcclusion", sceneView.AOTex);	// SSAO Tex
	}
	pRenderer->SetTexture("texDepth", pRenderer->GetDefaultDepthTargetTexture());

	pRenderer->SetSamplerState("sNearestSampler", EDefaultSamplerState::POINT_SAMPLER);
	pRenderer->SetConstant1f("ambientFactor", sceneView.AmbientFactor);
	pRenderer->SetVertexBuffer((int32)EGeometry::FULLSCREENQUAD);
	pRenderer->SetIndexBuffer((int32)EGeometry::FULLSCREENQUAD);
	pRenderer->Apply();
	pRenderer->DrawIndexed();	



	////PointLights, SpotLights
	//PointLights.resize(sceneView.PointLightCount);
	//SpotLights.resize(sceneView.SpotLightCount);
	//for (; iter != iterEnd; iter++)
	//{
	//	//Gathering Instanced Rendering Contexts~
	//	//Bind IA Pair 
	//	(*iter)->GatherLighting(sceneLightings,PointLights, SpotLights);
	//}
	

	
	pRenderer->SetViewport(pRenderer->GetAASetting().ResolutionX, pRenderer->GetAASetting().ResolutionY);
	pRenderer->SetShader(BRDFLightingShader);
	pRenderer->BindRenderTarget(ShaderRenderTarget);
	pRenderer->SetBlendState(EDefaultBlendState::ADDITIVE_COLOR);
	pRenderer->BeginRender(SClearCommand::Color(SVector4(0, 0, 0, 0.0f)));
	pRenderer->Apply();


	//mpRenderer->SetConstant2f("spotShadowMapDimensions", vec2(shadowDimension, shadowDimension));
	//mpRenderer->SetConstant1f("directionalShadowMapDimension", directionalShadowMapDimensions.x());
	
	if (pRenderManager->m_ShadowMapPass.m_ShadowMapTexture_Directional != INDEX_NONE)
		pRenderer->SetTextureArray("texDirectionalShadowMaps", pRenderManager->m_ShadowMapPass.m_ShadowMapTexture_Directional);

	if (pRenderManager->m_ShadowMapPass.m_ShadowMapTextures_Point != INDEX_NONE)
		pRenderer->SetTextureArray("texPointShadowMaps", pRenderManager->m_ShadowMapPass.m_ShadowMapTextures_Point);

	pRenderer->SetConstantStruct("Lights", (void*)&data.sceneLightings);
	if(data.sceneLightings.numPointLights >0)
		pRenderer->SetStructured("point_lights", data.PointLights.data(), sizeof(SPointLight) * data.sceneLightings.numPointLights);
	if (data.sceneLightings.numSpots > 0)
		pRenderer->SetStructured("spots", data.SpotLights.data(), sizeof(SSpotLight) * data.sceneLightings.numSpots);

	if (data.sceneLightings.numPointCasters > 0)
		pRenderer->SetStructured("point_casters", data.PointLightCasters.data(), sizeof(SPointLight) * data.sceneLightings.numPointCasters);
	

	pRenderer->SetSamplerState("sLinearSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
	pRenderer->SetSamplerState("sShadowSampler", EDefaultSamplerState::POINT_SAMPLER_WRAP_UVW);

	pRenderer->SetTexture("texDiffuseRoughnessMap", pRenderer->GetRenderTargetTexture(GBuffer.RTDiffuseRoughness));	
	pRenderer->SetTexture("texDepth", pRenderer->GetDefaultDepthTargetTexture());
	pRenderer->SetTexture("texSpecularMetalnessMap", pRenderer->GetRenderTargetTexture(GBuffer.RTSpecularMetallic));
	pRenderer->SetTexture("texNormals", pRenderer->GetRenderTargetTexture(GBuffer.RTNormals));
	pRenderer->SetTexture("texEmissiveMap", pRenderer->GetRenderTargetTexture(GBuffer.RTEmissive));	
	
	pRenderer->SetTexture("texAmbient", pRenderer->GetRenderTargetTexture(AmbientRenderTarget));
	
	
	pRenderer->SetConstant4x4f("matView", sceneView.matView);
	pRenderer->SetConstant4x4f("matViewToWorld", sceneView.matViewInverse);
	pRenderer->SetConstant4x4f("directionalProj", sceneView.DirectionalLightProjection);	/////
	pRenderer->SetConstant4x4f("matProjInverse", sceneView.matProjInverse);
	pRenderer->SetConstant1f("directionalShadowMapDimension", DIR_SHADOW_DIMENSION);
	pRenderer->SetConstant2f("spotShadowMapDimensions", POINT_SHADOW_DIMENSION);

	//texPointShadowMaps
	
	

	

	//Setup FullScreenQuad
	pRenderer->SetVertexBuffer((int32)EGeometry::FULLSCREENQUAD);
	pRenderer->SetIndexBuffer((int32)EGeometry::FULLSCREENQUAD);
	pRenderer->Apply();
	pRenderer->DrawIndexed();
}

void SDeferredRenderingPasses::RenderToneMapper(class Renderer* pRenderer, FRenderTargetID FinalTarget)
{
	pRenderer->SetShader(_toneMappingShaderHDR, true);

	pRenderer->UnbindDepthTarget();
	pRenderer->SetVertexBuffer((int32)EGeometry::FULLSCREENQUAD);
	pRenderer->SetIndexBuffer((int32)EGeometry::FULLSCREENQUAD);
	pRenderer->SetSamplerState("Sampler", blurSmp);
	pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_BACK);
	pRenderer->SetConstant1f("exposure", exposure);
	pRenderer->BindRenderTarget(FinalTarget);

	pRenderer->SetTexture("ColorTexture", pRenderer->GetRenderTargetTexture(ShaderRenderTarget));
	pRenderer->Apply();
	pRenderer->DrawIndexed();
}

void SDeferredRenderingPasses::RenderParticles(class Renderer* pRenderer, class RenderManager* pRenderManager, const SSceneView& sceneView, FRenderTargetID FinalTarget) const
{
	pRenderer->UnbindDepthTarget();
	pRenderer->UnbindRenderTargets();
	pRenderer->SetShader(EShaders::PARTICLE_RENDER, true);
	pRenderer->Apply();

	pRenderer->SetViewport(pRenderer->GetAASetting().ResolutionX, pRenderer->GetAASetting().ResolutionY);

	pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_NONE);
	pRenderer->SetBlendState(EDefaultBlendState::ALPHA_BLEND);
	pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED);
	pRenderer->BindRenderTarget(ShaderRenderTarget);

	FParticleList::const_iterator iter = sceneView.ParticleList.begin();
	FParticleList::const_iterator iter_end = sceneView.ParticleList.end();

	for(;iter != iter_end;iter++)
	{
		(*iter)->Render(pRenderer);
	}
	



}

FDepthStencilStateID SDeferredRenderingPasses::GeometryStencilState = INDEX_NONE;

FDepthStencilStateID SDeferredRenderingPasses::GeometryStencilStatePreZ = INDEX_NONE;

FShaderID SDeferredRenderingPasses::GeometryShader = INDEX_NONE;

FShaderID SDeferredRenderingPasses::GeometryInstancedShader = INDEX_NONE;

FShaderID SDeferredRenderingPasses::AmbientShader = INDEX_NONE;

FShaderID SDeferredRenderingPasses::AmbientIBLShader = INDEX_NONE;

FShaderID SDeferredRenderingPasses::BRDFLightingShader = INDEX_NONE;

FShaderID SDeferredRenderingPasses::_toneMappingShaderHDR = INDEX_NONE;

FSamplerID SDeferredRenderingPasses::blurSmp = INDEX_NONE;

bool SDeferredRenderingPasses::ShaderInitilized = false;

