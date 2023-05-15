#pragma once
#include "XObject/XObject.h"
#include "Renderer/RenderPass.h"
#include "Assets/Mesh/Mesh.h"
#include "Renderer/AmbientOcclusionPass.h"
#include <Renderer/ShadowMapPass.h>

//RenderPass 별로 그릴목록을 구성하여야함.
// Sky
//Opaque
//Transparent
//Light??
//Culling Frustum

static bool IsBoundingBoxVisibleFromFrustum(const SFrustumPlaneset& frustum, const SVector3& bMin , const SVector3& bMax);

static void GetAABBPoints(std::vector<SVector4>& Corners, const SVector3& bMin, const SVector3& bMax);

static bool IsSphereInFrustum(const SFrustumPlaneset& frustum, const SVector3& Center, float Radius);


class RenderManager :public XObject
{
	friend class Globals;
	friend class SceneComponent;
	friend class Engine;
	friend struct SDeferredRenderingPasses;

	using Super = XObject;
	


private:
	RenderManager();
	RenderManager(const RenderManager& Other);
	virtual ~RenderManager();


	//virtual void PreUpdate(float DeltaSeconds)	override;

	//virtual void Update(float DeltaSeconds)		override;

public:
	void Initialize();

	/*
	*	Cull Lights
	*  CULL MAIN VIEW RENDER LISTS
	*  CULL SHADOW VIEW RENDER LISTS
	*  SORT RENDER LISTS		
	*  BATCH SHADOW VIEW RENDER LISTS
	*   BATCH MAIN VIEW RENDER LISTS
	*  Prepare Render
	*/
	void PreRender();

	void Render();

private:
	void FrustumCull(const SFrustumPlaneset& frustumPlanes , const FRenderingList& InComponentsList, FRenderingList& OutCulledComponents) ;

	void CullShadowingLights(const SSceneView& sceneView ,SShadowView& outShadowView);
	

public:
	void AddRenderList(class SceneComponent* InSceneComp);


	const SDeferredRenderingPasses& GetDeferredPass() const
	{
		return m_DeferredPass;
	}

	FTextureID GetFinalTargetTextureID() const;

	void ClearRenderContext();




public:
	void DrawLine(const SVector3& pos1, const SVector3& pos2, const SLinearColor& color);
	void DrawSolid(const SVector3& pos1, const SVector3& pos2,  const SVector3& pos3, const SLinearColor& color);
	

private:
	//FRenderLayerMap	m_mapRenderList;
	
	
	SSceneView		m_SceneView;

	SShadowView		m_ShadowView;

	SFrameLightingData	m_FrameLightingData;
private:
	SDeferredRenderingPasses	m_DeferredPass;	///
	class Renderer*				m_cachedRenderer;

	FRenderingList			m_FrameRenderList;
	FParticleList			m_FrameParticleList;
	FLightingList			m_FrameLightingList;

	AmbientOcclusionPass		m_AmbientOcclusionPass;	
	
	SShadowMapPass					m_ShadowMapPass;

	FRenderTargetID					m_FinalTarget;


	FRenderTargetID					m_UIRenderTarget;


private:
	Mesh			m_FullScreenQuad;

	Mesh			m_TestQuad;


	std::queue<SDrawLineCommand>		m_DrawLineCommands;

	std::queue<SDrawSolidCommand>		m_DrawSolidCommands;



	
	

};

