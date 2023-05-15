#include "EnginePCH.h"
#include "RenderManager.h"
#include "GameFramework/Components/SceneComponent.h"
#include "GameFramework/Components/SpotLightComponent.h"
#include "GameFramework/Components/PointLightComponent.h"
#include "GameFramework/Components/CameraComponent.h"
#include "Renderer/Renderer.h"
#include "D3D/D3DManager.h"
#include "Renderer/GeometryGenerator.h"
#include "Assets/Mesh/MeshAsset.h"
#include "Assets/Material/Material.h"
#include "CameraManager.h"
#include "AssetManager.h"
#include "Path/PathManager.h"
#include "GameFramework/Components/SkyComponent.h"
#include <GameFramework/Components/PrimitiveComponent.h>
#include "SceneViewport.h"
#include <UI/UIManager.h>







#define RENDER_THRESHLD 3

RenderManager::RenderManager()
	:m_cachedRenderer(nullptr)
	, m_FinalTarget(INDEX_NONE)
	,m_UIRenderTarget(INDEX_NONE)
{

}

RenderManager::RenderManager(const RenderManager& Other)
	: Super(Other)
{

}

RenderManager::~RenderManager()
{
	m_FrameRenderList.clear();
	//m_SceneView.AlphaList.clear();
	//m_SceneView.OpaqueList.clear();
}

void RenderManager::Initialize()
{
	m_cachedRenderer = g_Engine->Get<Renderer>();

	m_TestQuad = GeometryGenerator::Quad(0.5f);
	m_FullScreenQuad = GeometryGenerator::FullScreenQuad();

	
	m_DeferredPass.Initialize(m_cachedRenderer, true);

	m_AmbientOcclusionPass.Initialize(m_cachedRenderer);
	

	SRenderTargetDesc rtDesc_f4;	
	rtDesc_f4.TextureDesc.Width = m_cachedRenderer->GetAASetting().ResolutionX;
	rtDesc_f4.TextureDesc.Height = m_cachedRenderer->GetAASetting().ResolutionY;
	rtDesc_f4.TextureDesc.mipCount = 1;
	rtDesc_f4.TextureDesc.arraySize = 1;
	rtDesc_f4.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
	rtDesc_f4.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtDesc_f4.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	
	m_ShadowMapPass.Initialize(m_cachedRenderer);

	m_FinalTarget = m_cachedRenderer->AddRenderTarget(rtDesc_f4);

	m_UIRenderTarget = m_cachedRenderer->AddRenderTarget(rtDesc_f4);
}

//PostUpdate			//PushRenderList
//PreRender				//CullPushedRenderList
//Render				//Render!

void RenderManager::PreRender()
{
	//Make SceneView..
	//m_DeferredPass.
	/*
	*	Cull Lights
	*  CULL MAIN VIEW RENDER LISTS
	*  CULL SHADOW VIEW RENDER LISTS
	*  SORT RENDER LISTS
	*  BATCH SHADOW VIEW RENDER LISTS
	*   BATCH MAIN VIEW RENDER LISTS
	*  Prepare Render
	*/
	

	/**
	 * Make Rendering List
	 */
	//FRenderCompList renderListInFrame;	
	//이번 프레임에 그릴 준비를합니다.
	//카메라, 조명 오브젝트등을 구성합니다.
	
	//CullRenderListss...
	//CullLights..
	CameraManager* camMgr = g_Engine->Get<CameraManager>();

	m_SceneView.matProj = m_SceneView.matProjInverse = camMgr->GetCurrentCamera()->GetProjMatrix();;
	m_SceneView.matProjInverse.Inverse();

	m_SceneView.matView = m_SceneView.matViewInverse = camMgr->GetCurrentCamera()->GetViewMatrix();
	m_SceneView.matViewInverse.Inverse();

	m_SceneView.matViewProj = m_SceneView.matView * m_SceneView.matProj;
	m_SceneView.CameraPosition = camMgr->GetCurrentCamera()->GetWorldLocation();

	m_ShadowView.viewMatrix = m_SceneView.matView;

	if (m_ShadowView.Directional)
	{
		m_SceneView.DirectionalLightProjection = m_ShadowView.Directional->GetProjectionMatrix();
	}
	

	FLightingList::const_iterator liter = m_FrameLightingList.begin();
	FLightingList::const_iterator liter_end = m_FrameLightingList.end();

	for (; liter != liter_end; liter++)
	{
		m_SceneView.LightingList.push_back(*liter);
	}	

	FRenderingList culledList;
	FrustumCull(SFrustumPlaneset::ExtractFromMatrix(m_SceneView.matViewProj), m_FrameRenderList, OUT culledList);


	FParticleList::const_iterator piter = m_FrameParticleList.begin();
	FParticleList::const_iterator piter_end = m_FrameParticleList.end();

	for (; piter != piter_end; piter++)
	{
		m_SceneView.ParticleList.push_back(*piter);
	}

	
	

	//For ShadowPass

	CullShadowingLights(m_SceneView,OUT m_ShadowView);
	
	
	


	//For MainPass
	//Gathering Light Data	 
	{
		FLightingList::const_iterator iter = m_SceneView.LightingList.begin();
		FLightingList::const_iterator iterEnd = m_SceneView.LightingList.end();

		//PointLights, SpotLights
		m_FrameLightingData.PointLights.resize(m_SceneView.PointLightCount);
		m_FrameLightingData.PointLightCasters.resize(m_SceneView.PointLightCasterCount);
		m_FrameLightingData.SpotLights.resize(m_SceneView.SpotLightCount);

		for (; iter != iterEnd; iter++)
		{
			//Gathering Instanced Rendering Contexts~
			//Bind IA Pair 
			(*iter)->GatherLighting(OUT m_FrameLightingData.sceneLightings, OUT m_FrameLightingData.PointLights,OUT m_FrameLightingData.PointLightCasters, OUT m_FrameLightingData.SpotLights);
		}
	}


	FRenderingList::const_iterator iter = culledList.begin();
	FRenderingList::const_iterator iter_end = culledList.end();
	for (; iter != iter_end; iter++)
	{
		MeshAsset* meshAsset = (*iter)->GetMeshAsset();
		if(meshAsset)
		{
			if(meshAsset->GetRenderCount() > RENDER_THRESHLD)
			{
				//*iter				
				m_SceneView.InstanceList[meshAsset].push_back(*iter);
				m_ShadowView.Casters_Inst[meshAsset].push_back(*iter);
				continue;
			}
		}		

		m_SceneView.OpaqueList.push_back(*iter);		
		m_ShadowView.Casters.push_back(*iter);
	}

	//m_SceneView.OpaqueList.insert( m_FrameRenderList.begin(), m_FrameRenderList.end()) ;	


	//m_FrameRenderList.clear();	//이번프레임에 그릴 목록을 그릴 카테고리에맞게 분배하고 clear 합니다
	//m_FrameLightingList.clear();
}

void RenderManager::Render()
{
	//Shadow
	m_cachedRenderer->UnbindRenderTargets();
	m_ShadowMapPass.RenderShadowMaps(m_cachedRenderer, m_ShadowView);



	m_DeferredPass.RenderGBuffer(m_cachedRenderer, m_SceneView);		
	m_AmbientOcclusionPass.RenderAmbientOcclusion(m_cachedRenderer, m_cachedRenderer->GetRenderTargetTexture(m_DeferredPass.GBuffer.RTNormals), m_SceneView);
	
	m_SceneView.AOTex = m_cachedRenderer->GetRenderTargetTexture(m_AmbientOcclusionPass.gaussianBlurRenderTarget);

	m_DeferredPass.RenderLightingPass(m_cachedRenderer, this ,m_SceneView, m_FrameLightingData);
	if (m_SceneView.SceneSky)
	{
		m_SceneView.SceneSky->Render(m_cachedRenderer);
	}	
	//Post Process
	
	m_DeferredPass.RenderParticles(m_cachedRenderer, this, m_SceneView, m_FinalTarget);
	m_DeferredPass.RenderToneMapper(m_cachedRenderer, m_FinalTarget);

	

	//Draw DebugLines	

	m_cachedRenderer->SetShader(EShaders::LINE);
	m_cachedRenderer->BindRenderTarget(m_FinalTarget);
	m_cachedRenderer->BindDepthTarget(m_cachedRenderer->GetDefaultDepthTarget());
	m_cachedRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_WRITE);
	FTextureID texID = m_cachedRenderer->GetRenderTargetTexture(m_DeferredPass.ShaderRenderTarget);

	
	m_cachedRenderer->SetConstant4x4f("view", m_SceneView.matView);
	m_cachedRenderer->SetConstant4x4f("proj", m_SceneView.matProj);
	
	std::vector<SlineInfo>		lines;
	std::vector<SSolidInfo>		solides;
	lines.resize(m_DrawLineCommands.size());
	solides.resize(m_DrawSolidCommands.size());

	int32 line_idx = 0;
	int32 solide_idx = 0;

	while (m_DrawLineCommands.size() > 0)
	{
		SDrawLineCommand& cmd = m_DrawLineCommands.front();
		cmd.SetConstants(lines, line_idx);
		m_DrawLineCommands.pop();
		line_idx++;
	}

	if(line_idx)
	{
		m_cachedRenderer->SetStructured("lines", lines.data(), sizeof(SlineInfo)*lines.size());
		m_cachedRenderer->Apply();
		m_cachedRenderer->DrawInstanced(1, line_idx);
	}
	
	m_cachedRenderer->SetShader(EShaders::SOLID);		
	m_cachedRenderer->BindRenderTarget(m_FinalTarget);

	m_cachedRenderer->SetConstant4x4f("view", m_SceneView.matView);
	m_cachedRenderer->SetConstant4x4f("proj", m_SceneView.matProj);

	while (m_DrawSolidCommands.size() > 0)
	{
		SDrawSolidCommand& cmd = m_DrawSolidCommands.front();
		cmd.SetConstants(solides, solide_idx);
		m_DrawSolidCommands.pop();
		solide_idx++;
	}

	if (solide_idx)
	{
		m_cachedRenderer->SetStructured("solids", solides.data(), sizeof(SSolidInfo) * solides.size());
		m_cachedRenderer->Apply();
		m_cachedRenderer->DrawInstanced(1, solide_idx);
	}




	//drawUI
	//UIManager의 PanelTarget에 그림
	g_Engine->Get<SceneViewport>()->Render(m_cachedRenderer);

	
	

	//UIPASS
	m_cachedRenderer->SetShader(EShaders::UNLIT, true);
	m_cachedRenderer->BindRenderTarget(m_FinalTarget);
	m_cachedRenderer->UnbindDepthTarget();
	m_cachedRenderer->Apply();


	STextureInfo tex;
	tex.alpha = 1.0f;
	tex.diffuse = SVector3(1.0f);
	tex.isDiffuseMap = 1.0f;
	m_cachedRenderer->SetConstantStruct("texInfo", &tex);
	m_cachedRenderer->SetTexture("texDiffuseMap", g_Engine->Get<UIManager>()->GetUITargetTexture());
	m_cachedRenderer->SetVertexBuffer((int32)EGeometry::FULLSCREENQUAD);
	m_cachedRenderer->SetIndexBuffer((int32)EGeometry::FULLSCREENQUAD);
	m_cachedRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_NONE);
	m_cachedRenderer->Apply();
	m_cachedRenderer->DrawIndexed();



	m_SceneView.ParticleList.clear();
	m_SceneView.OpaqueList.clear();	//다그렸음으로 모든 리스트를 클리어합니다.
	m_SceneView.InstanceList.clear();
	m_SceneView.LightingList.clear();
	m_SceneView.PointLightCount = m_SceneView.SpotLightCount = 0;
	m_SceneView.PointLightCasterCount = 0;
	m_SceneView.SceneSky = nullptr;
	
	m_FrameParticleList.clear();
	m_FrameRenderList.clear();
	m_FrameLightingList.clear();
	//Clear FrameLight Datas.
	m_FrameLightingData.sceneLightings.Clear();	
	m_FrameLightingData.sceneLightings.directional.enabled = false;
	m_ShadowView.Clear();

	
}

/*MainView*/
void RenderManager::FrustumCull(const SFrustumPlaneset& frustumPlanes, const FRenderingList& InComponentsList, FRenderingList& OutCulledComponents) 
{
	FRenderingList::const_iterator iter = InComponentsList.begin();
	FRenderingList::const_iterator iter_end = InComponentsList.end();

	for(;iter != iter_end;iter++)
	{
		SVector3 world_minAABB;
		SVector3 world_maxAABB;
		(*iter)->GetAABB(OUT world_minAABB, OUT world_maxAABB);
		if(IsBoundingBoxVisibleFromFrustum(frustumPlanes, world_minAABB, world_maxAABB))
		{
			OutCulledComponents.push_back((*iter));
		}
	}
}


void RenderManager::CullShadowingLights(const SSceneView& sceneView, SShadowView& outShadowView)
{
	FLightingList::const_iterator iter = sceneView.LightingList.begin();
	FLightingList::const_iterator iter_end = sceneView.LightingList.end();
	
	for(;iter != iter_end;iter++)
	{		
		if(!(*iter)->IsCastingShadow())
			continue;

		if((*iter)->GetClass() == PointLightComponent::StaticClass())
		{
			PointLightComponent* pointLight = (PointLightComponent*)(*iter);

			SVector3 vecCamera = sceneView.CameraPosition - (*iter)->GetWorldLocation();

			const float dstSqrCameraToLight = XMVector3Dot(vecCamera.Convert(), vecCamera.Convert()).m128_f32[0];
			const float rangeSqr = pointLight->GetLightRange() * pointLight->GetLightRange();

			const bool bIsCameraInPointLightSphereOfIncluence = dstSqrCameraToLight < rangeSqr;
			//Cam to View Frustum
			const bool bSphereInFrustum = IsSphereInFrustum(SFrustumPlaneset::ExtractFromMatrix(sceneView.matViewProj) ,(*iter)->GetWorldLocation(),pointLight->GetLightRange());

			if (bIsCameraInPointLightSphereOfIncluence || bSphereInFrustum)
			{
				outShadowView.Points.push_back((*iter));				
				
			} 
		}
		//Spot Light
		else if ((*iter)->GetClass() == SpotLightComponent::StaticClass())
		{
			outShadowView.Spots.push_back((*iter));			
		}
		else
		{
			outShadowView.Directional = (*iter);
		}
	}	

}

//매프래임 갱신합니다.
void RenderManager::AddRenderList(class SceneComponent* InSceneComp)
{	
	if (InSceneComp->GetClass()->IsA(LightComponent::StaticClass())
		|| InSceneComp->GetClass()->IsChildOf(LightComponent::StaticClass()))
	{
		LightComponent* lightComp = (LightComponent*)InSceneComp;
		m_FrameLightingList.push_back(lightComp);

		if (InSceneComp->GetClass()->IsA(SpotLightComponent::StaticClass()))
		{
			m_SceneView.SpotLightCount++;

		} else if (InSceneComp->GetClass()->IsA(PointLightComponent::StaticClass()))
		{
			m_SceneView.PointLightCount++;

			if (lightComp->IsCastingShadow())
				m_SceneView.PointLightCasterCount++;
		} else
		{
			m_ShadowView.Directional = (LightComponent*)InSceneComp;
		}
	}
	else if (InSceneComp->GetClass()->IsA(SkyComponent::StaticClass()))
	{
		m_SceneView.SceneSky = (SkyComponent*)InSceneComp;
		m_SceneView.environmentMap = m_SceneView.SceneSky->m_EnvMap;
	}
	else if(InSceneComp->GetClass()->IsA(ParticleComponent::StaticClass()))
	{
		m_FrameParticleList.push_back((ParticleComponent*)InSceneComp);
	}
	else if (InSceneComp->GetClass()->IsA(PrimitiveComponent::StaticClass())
			   || InSceneComp->GetClass()->IsChildOf(PrimitiveComponent::StaticClass()))
	{
		m_FrameRenderList.push_back(InSceneComp);
	}	
}

FTextureID RenderManager::GetFinalTargetTextureID() const
{
	return m_cachedRenderer->GetRenderTargetTexture(m_FinalTarget);
}

void RenderManager::ClearRenderContext()
{
	m_SceneView.OpaqueList.clear();	//다그렸음으로 모든 리스트를 클리어합니다.
	m_SceneView.InstanceList.clear();
	m_SceneView.LightingList.clear();
	m_SceneView.PointLightCount = m_SceneView.SpotLightCount = 0;
	m_SceneView.PointLightCasterCount = 0;
	m_SceneView.SceneSky = nullptr;
	m_FrameRenderList.clear();	//이번프레임에 그릴 목록을 그릴 카테고리에맞게 분배하고 clear 합니다
	m_FrameLightingList.clear();
	m_FrameParticleList.clear();


	//Clear FrameLight Datas.
	m_FrameLightingData.sceneLightings.Clear();
	m_FrameLightingData.sceneLightings.directional.enabled = false;
	m_ShadowView.Clear();
}

void RenderManager::DrawLine(const SVector3& pos1, const SVector3& pos2, const SLinearColor& color)
{
	if(g_Engine->IsEditMode())
		m_DrawLineCommands.push(SDrawLineCommand(pos1, pos2, color));
}

void RenderManager::DrawSolid(const SVector3& pos1, const SVector3& pos2,  const SVector3& pos3, const SLinearColor& color)
{
	if (g_Engine->IsEditMode())
		m_DrawSolidCommands.push(SDrawSolidCommand(pos1, pos2, pos3, color.ToVector3()));
}

bool IsBoundingBoxVisibleFromFrustum(const SFrustumPlaneset& frustum, const SVector3& bMin, const SVector3& bMax)
{
	std::vector<SVector4> boxPoints;
	boxPoints.resize(8);
	GetAABBPoints(OUT boxPoints, bMin, bMax);

	constexpr float EPSILON = 0.000002f;

	for (int32 i = 0; i < 6; ++i)	// for each plane
	{
		bool bInside = false;
		for (int32 j = 0; j < 8; ++j)	// for each point
		{
			if (XMVector4Dot(boxPoints[j].Convert(), frustum.abcd[i].Convert()).m128_f32[0] > EPSILON)
			{
				bInside = true;
				break;
			}
		}
		if (!bInside)
		{
			return false;
		}
	}

	return true;
}

void GetAABBPoints(std::vector<SVector4>& Corners, const SVector3& bMin, const SVector3& bMax)
{
	Corners[0] = SVector4(bMin.x, bMin.y, bMin.z,1.0f);
	Corners[1] = SVector4(bMax.x, bMin.y, bMin.z,1.0f);
	Corners[2] = SVector4(bMax.x, bMax.y, bMin.z,1.0f);
	Corners[3] = SVector4(bMin.x, bMax.y, bMin.z,1.0f);
	Corners[4] = SVector4(bMin.x, bMin.y, bMax.z,1.0f);	
	Corners[5] = SVector4(bMax.x, bMin.y, bMax.z,1.0f);
	Corners[6] = SVector4(bMax.x, bMax.y, bMax.z,1.0f);
	Corners[7] = SVector4(bMin.x, bMax.y, bMax.z,1.0f);
	
	
}

bool IsSphereInFrustum(const SFrustumPlaneset& frustum, const SVector3& Center, float Radius)
{
	bool bInside = true;
	for (int plane = 0; plane < 6; ++plane)
	{
		SVector3 N = SVector3(
			frustum.abcd[plane].x
			, frustum.abcd[plane].y
			, frustum.abcd[plane].z
		);
		const SVector3 planeNormal = N.GetSafeNormal();
		const float D = XMVector3Dot(N.Convert(), Center.Convert()).m128_f32[0] + frustum.abcd[plane].w;
		//const float D = XMVector3Dot(planeNormal, sphere.c).m128_f32[0] + frustum.abcd[plane].w;
		//const float D = XMVector3Dot(planeNormal, sphere.c).m128_f32[0];
				
		
		if (D < 0.0f)
		{
			//if ( (-D -frustum.abcd[plane].w) > sphere.r)
			if (-D > Radius)
				return false;
		}
	}

	return bInside;
}
