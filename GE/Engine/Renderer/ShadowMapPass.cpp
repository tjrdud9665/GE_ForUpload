#include "EnginePCH.h"
#include "ShadowMapPass.h"
#include "Renderer.h"
#include <GameFramework/Components/LightComponent.h>
#include <Assets/Mesh/SkeletalMeshAsset.h>

#define NUM_POINT_LIGHT 100
#define NUM_POINT_LIGHT_SHADOW 3

#define NUM_SPOT_LIGHT 20
#define NUM_SPOT_LIGHT_SHADOW 5

void SShadowMapPass::Initialize(class Renderer* pRenderer)
{
	m_Renderer = pRenderer;
	m_ShadowMapShader = EShaders::SHADOWMAP_DEPTH;
	m_ShadowMapShaderInstanced = EShaders::SHADOWMAP_DEPTH_INSTANCED;		

	const SShaderDesc cubemapDepthShaderDesc =
	{ "ShadowCubeMapShader",
		{
		SShaderStageDesc{ "ShadowCubeMapShader_vs.hlsl", {} },
		SShaderStageDesc{ "ShadowCubeMapShader_ps.hlsl", {} }
		}
	};
	const std::vector<SShaderMacro> instancedShaderMacros =
	{
		SShaderMacro{ "INSTANCED", "1" }

	};
	
	const SShaderDesc InstancedcubemapDepthShaderDesc= 
	{ "InstancedShadowCubeMapShader",
	 {
		 SShaderStageDesc{ "ShadowCubeMapShader_vs.hlsl", instancedShaderMacros },
		 SShaderStageDesc{ "ShadowCubeMapShader_ps.hlsl", instancedShaderMacros }
	} };

	m_ShadowCubeMapShader = m_Renderer->CreateShader(cubemapDepthShaderDesc);
	m_ShadowCubeMapShaderInstanced = m_Renderer->CreateShader(InstancedcubemapDepthShaderDesc);

	m_ShadowMapDimension_Spot = 1024;
	m_ShadowMapDimension_Point = 2048;
	m_ShadowMapDimension_Direction = 4096;


	InitializeDirectionalLightShadowMap();
	InitializePointLightShadowMaps();
	InitializeSpotLightShadowMaps();
	
};

void SShadowMapPass::RenderShadowMaps(Renderer* pRenderer, const SShadowView& shadowView) 
{
	D3D11_VIEWPORT viewPort = {};
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;

	const bool bNoShadowingLights = shadowView.Spots.empty() && shadowView.Points.empty() && (shadowView.Directional == nullptr || !shadowView.Directional->IsCastingShadow());
	if (bNoShadowingLights)
		return;

	pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_WRITE);
	pRenderer->SetShader(m_ShadowMapShader); // shader for rendering z buffer

	// CLEAR SHADOW MAPS
	//
	for (size_t i = 0; i < shadowView.Spots.size(); i++)
	{
		pRenderer->BindDepthTarget(m_DepthTargets_Spot[i]);	// only depth stencil buffer
		pRenderer->BeginRender(SClearCommand::Depth(1.0f));
	}
	for (size_t i = 0; i < shadowView.Points.size(); i++)
	{
		for (int face = 0; face < 6; ++face)
		{
			const size_t depthTargetIndex = i * 6 + face;
			pRenderer->BindDepthTarget(m_DepthTargets_Point[depthTargetIndex]);	// only depth stencil buffer
			pRenderer->BeginRender(SClearCommand::Depth(1.0f));
		}
	}

	//-----------------------------------------------------------------------------------------------
// DIRECTIONAL SHADOW MAP
//-----------------------------------------------------------------------------------------------
	pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_FRONT);
	if (shadowView.Directional != nullptr && shadowView.Directional->IsCastingShadow())
	{		
		const Matrix viewProj =  shadowView.Directional->GetLightSpaceMatrix();

		// RENDER NON-INSTANCED SCENE OBJECTS
		//
		const int32 shadowMapDimension = pRenderer->GetTextureObject(pRenderer->GetDepthTargetTexture(m_DepthTarget_Directional))->GetWidth();
		viewPort.Height = static_cast<float>(shadowMapDimension);
		viewPort.Width = static_cast<float>(shadowMapDimension);
		pRenderer->SetViewport(viewPort);
		pRenderer->BindDepthTarget(m_DepthTarget_Directional);
		pRenderer->Apply();
		pRenderer->BeginRender(SClearCommand::Depth(1.0f));
		FRenderingList::const_iterator iter = shadowView.Casters.begin();
		FRenderingList::const_iterator iter_end = shadowView.Casters.end();
		
		//non_instancing 
		for(;iter != iter_end;iter++)
		{	
			(*iter)->RenderDepth(pRenderer,viewProj);					
		}

	

		if (shadowView.Casters_Inst.size() != 0)
		{
			pRenderer->SetShader(m_ShadowMapShaderInstanced);


			{
				FInstancingRenderList::const_iterator iter = shadowView.Casters_Inst.begin();
				FInstancingRenderList::const_iterator iterEnd = shadowView.Casters_Inst.end();

			for (; iter != iterEnd; iter++)
			{
					//Gathering Instanced Rendering Contexts~
					//Bind IA Pair 
					FRenderingList::const_iterator iter_perMesh = (*iter).second.begin();
					FRenderingList::const_iterator iter_perMesh_end = (*iter).second.end();

					std::vector<SDepthInstanceInfo> instanceInfo;
					instanceInfo.resize((*iter).second.size());
					int32 InstanceID = 0;

					for (; iter_perMesh != iter_perMesh_end; iter_perMesh++, InstanceID++)
					{
						(*iter_perMesh)->GatherRenderDeptthInstance(pRenderer,viewProj,instanceInfo, InstanceID);
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

							std::vector<SObjectShadowMatrices> matrices;							
							std::vector<int32>	useSkinning;
							std::vector<SVector3> rootOffset;
							matrices.resize(InstanceID);							
							useSkinning.resize(InstanceID);
							rootOffset.resize(InstanceID);
							for (int32 i = 0; i < InstanceID; i++)
							{
								matrices[i] = instanceInfo[i].objMatrics;								
								useSkinning[i] = instanceInfo[i].useSkinning;
								rootOffset[i] = instanceInfo[i].rootOffset;
							}						

							if ((*iter).first->GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
							{
								SkeletalMeshAsset* skeletalMesh = (SkeletalMeshAsset*)((*iter).first);
								if (skeletalMesh->GetInstancingBoneMatrixArray() != INDEX_NONE)
									pRenderer->SetStructured("InstancingComputedMatrix", skeletalMesh->GetInstancingBoneMatrixArray());
							}

							pRenderer->SetStructured("InstancedObjMatrices", matrices.data(), sizeof(SObjectShadowMatrices) * instanceInfo.size());
							pRenderer->SetStructured("InstancedUseSkinning", useSkinning.data(), sizeof(int32) * instanceInfo.size());
							pRenderer->SetStructured("InstancedRootOffset", rootOffset.data(), sizeof(SVector3) * instanceInfo.size());
							pRenderer->Apply();
							pRenderer->DrawIndexedInstanced(InstanceID);

						}
					}


				}
			}

		}	
	}


	//-----------------------------------------------------------------------------------------------
	// POINT LIGHT SHADOW MAPS
	//-----------------------------------------------------------------------------------------------
	{
		viewPort.Height = static_cast<float>(m_ShadowMapDimension_Point);
		viewPort.Width = static_cast<float>(m_ShadowMapDimension_Point);

		pRenderer->SetShader(m_ShadowCubeMapShader);
		pRenderer->SetViewport(viewPort);
		pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_NONE);

		{
			FLightingList::const_iterator iter = shadowView.Points.begin();
			FLightingList::const_iterator iter_end = shadowView.Points.end();
			SPointLightBuffer cbPointLight;
			size_t i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				//cbPointLight
				cbPointLight.LightPosition = (*iter)->GetWorldLocation();
				cbPointLight.FarPlane = (*iter)->GetLightRange();

				for (int face = 0; face < 6; ++face)
				{
					const Matrix viewProj =
						(*iter)->GetViewMatrix(static_cast<D3DTexture::CubemapUtility::ECubeMapLookDirections>(face)) * (*iter)->GetProjectionMatrix();

					const size_t depthTargetIndex = i * 6 + face;
					pRenderer->BindDepthTarget(m_DepthTargets_Point[depthTargetIndex]);	// only depth stencil buffer
					pRenderer->SetConstantStruct("cbLight", &cbPointLight);
					pRenderer->Apply();
					FRenderingList::const_iterator iter = shadowView.Casters.begin();
					FRenderingList::const_iterator iter_end = shadowView.Casters.end();

					//non_instancing 
					FRenderingList::const_iterator caster_iter = shadowView.Casters.begin();
					FRenderingList::const_iterator caster_iter_end = shadowView.Casters.end();

					//non_instancing 
					for (; caster_iter != caster_iter_end; caster_iter++)
					{
						(*caster_iter)->RenderDepth(pRenderer, viewProj);
					}
				}
			}
		}

		pRenderer->SetShader(m_ShadowCubeMapShaderInstanced);
		{
			FLightingList::const_iterator iter = shadowView.Points.begin();
			FLightingList::const_iterator iter_end = shadowView.Points.end();
			SPointLightBuffer cbPointLight;
			size_t i = 0;
			for (; iter != iter_end; iter++,i++)
			{
				//cbPointLight
				cbPointLight.LightPosition = (*iter)->GetWorldLocation();
				cbPointLight.FarPlane = (*iter)->GetLightRange();

				for (int face = 0; face < 6; ++face)
				{
					const Matrix viewProj =
						(*iter)->GetViewMatrix(static_cast<D3DTexture::CubemapUtility::ECubeMapLookDirections>(face)) * (*iter)->GetProjectionMatrix();

					const size_t depthTargetIndex = (i % NUM_POINT_LIGHT_SHADOW) * 6 + face;
				
					pRenderer->BindDepthTarget(m_DepthTargets_Point[depthTargetIndex]);	// only depth stencil buffer
					pRenderer->SetConstantStruct("cbLight", &cbPointLight);
					pRenderer->Apply();
					FRenderingList::const_iterator iter = shadowView.Casters.begin();
					FRenderingList::const_iterator iter_end = shadowView.Casters.end();


					FInstancingRenderList::const_iterator caster_inst_iter = shadowView.Casters_Inst.begin();
					FInstancingRenderList::const_iterator caster_inst_iter_end = shadowView.Casters_Inst.end();
					for (; caster_inst_iter != caster_inst_iter_end; caster_inst_iter++)
					{
						//Gathering Instanced Rendering Contexts~
						//Bind IA Pair 
						FRenderingList::const_iterator iter_perMesh = (*caster_inst_iter).second.begin();
						FRenderingList::const_iterator iter_perMesh_end = (*caster_inst_iter).second.end();

						std::vector<SDepthInstanceInfo> instanceInfo;
						instanceInfo.resize((*caster_inst_iter).second.size());
						int32 InstanceID = 0;

						for (; iter_perMesh != iter_perMesh_end; iter_perMesh++, InstanceID++)
						{
							(*iter_perMesh)->GatherRenderDeptthInstance(pRenderer, viewProj, instanceInfo, InstanceID);
						}

						int32 MeshCount = (*caster_inst_iter).first->GetMeshCount();

						for (int32 i = 0; i < MeshCount; i++)
						{
							size_t pairCount = (*caster_inst_iter).first->GetMesh(i).GetIAPairSize();

							for (size_t a = 0; a < pairCount; a++)
							{
								FIAPair iaPair = (*caster_inst_iter).first->GetMesh(i).GetIAPair((int32)a);
								pRenderer->SetVertexBuffer(iaPair.first);
								pRenderer->SetIndexBuffer(iaPair.second);

								std::vector<SObjectShadowMatrices> matrices;
								std::vector<int32>	useSkinning;
								std::vector<SVector3>	rootOffset;
								matrices.resize(InstanceID);
								useSkinning.resize(InstanceID);
								rootOffset.resize(InstanceID);
								for (int32 i = 0; i < InstanceID; i++)
								{
									matrices[i] = instanceInfo[i].objMatrics;
									useSkinning[i] = instanceInfo[i].useSkinning;
									rootOffset[i] = instanceInfo[i].rootOffset;
								}

								if ((*caster_inst_iter).first->GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
								{
									SkeletalMeshAsset* skeletalMesh = (SkeletalMeshAsset*)((*caster_inst_iter).first);
									if (skeletalMesh->GetInstancingBoneMatrixArray() != INDEX_NONE)
										pRenderer->SetStructured("InstancingComputedMatrix", skeletalMesh->GetInstancingBoneMatrixArray());
								}

								pRenderer->SetStructured("InstancedObjMatrices", matrices.data(), sizeof(SObjectShadowMatrices) * instanceInfo.size());
								pRenderer->SetStructured("InstancedUseSkinning", useSkinning.data(), sizeof(int32) * instanceInfo.size());
								pRenderer->SetStructured("InstancedRootOffset", rootOffset.data(), sizeof(SVector3)* instanceInfo.size());
								pRenderer->Apply();
								pRenderer->DrawIndexedInstanced(InstanceID);

							}
						}
					}
				}
			}

		}
	
	}



}

SVector2 SShadowMapPass::GetDirectionalShadowMapDimensions(Renderer* pRenderer) const
{
	if (m_DepthTarget_Directional == INDEX_NONE)
		return SVector2(0, 0);

	const float dim = static_cast<float>(pRenderer->GetTextureObject(pRenderer->GetDepthTargetTexture(m_DepthTarget_Directional))->GetWidth());
	return SVector2(dim);
}

void SShadowMapPass::InitializeSpotLightShadowMaps()
{
	const bool bDepthOnly = true;	

	SDepthTargetDesc depthDesc;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;

	STextureDesc& texDesc = depthDesc.TextureDesc;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = static_cast<ETextureUsage>(DEPTH_TARGET | RESOURCE);
	texDesc.Height = texDesc.Width = m_ShadowMapDimension_Spot;
	texDesc.arraySize = NUM_SPOT_LIGHT_SHADOW;

	// CREATE DEPTH TARGETS: SPOT LIGHTS
	//
	std::vector<FDepthTargetID> DepthTargetIDs = m_Renderer->AddDepthTarget(depthDesc);
	m_DepthTargets_Spot.resize(NUM_SPOT_LIGHT_SHADOW);
	
	std::copy(RANGE(DepthTargetIDs), m_DepthTargets_Spot.begin());

	m_ShadowMapTextures_Spot = m_DepthTargets_Spot.size() > 0
		? m_Renderer->GetDepthTargetTexture(m_DepthTargets_Spot[0])
		: -1;
}

void SShadowMapPass::InitializePointLightShadowMaps()
{	

	// check feature support & error handle:
	// https://msdn.microsoft.com/en-us/library/windows/apps/dn263150

	// CREATE DEPTH TARGETS: POINT LIGHTS
	//
	SDepthTargetDesc depthDesc;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;

	STextureDesc& texDesc = depthDesc.TextureDesc;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = static_cast<ETextureUsage>(DEPTH_TARGET | RESOURCE);
	texDesc.bIsCubeMap = true;
	texDesc.Height = texDesc.Width = m_ShadowMapDimension_Point;
	texDesc.arraySize = NUM_POINT_LIGHT_SHADOW;
	texDesc.FileName = "Point Light Shadow Maps";

	std::vector<FDepthTargetID> DepthTargetIDs = m_Renderer->AddDepthTarget(depthDesc);
	m_DepthTargets_Point.resize(NUM_POINT_LIGHT_SHADOW * 6 /*each face = one depth target*/);
	std::copy(RANGE(DepthTargetIDs), m_DepthTargets_Point.begin());

	m_ShadowMapTextures_Point = m_DepthTargets_Point.size() > 0 ? m_Renderer->GetDepthTargetTexture(m_DepthTargets_Point[0]) : -1;
}

void SShadowMapPass::InitializeDirectionalLightShadowMap()
{


	SDepthTargetDesc depthDesc;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;

	STextureDesc& texDesc = depthDesc.TextureDesc;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = static_cast<ETextureUsage>(DEPTH_TARGET | RESOURCE);
	texDesc.Height = texDesc.Width = m_ShadowMapDimension_Direction;
	texDesc.arraySize = 1;

	// first time - add target
	if (m_DepthTarget_Directional == INDEX_NONE)
	{
		m_DepthTarget_Directional = m_Renderer->AddDepthTarget(depthDesc)[0];
		m_ShadowMapTexture_Directional = m_Renderer->GetDepthTargetTexture(m_DepthTarget_Directional);
	}
}

