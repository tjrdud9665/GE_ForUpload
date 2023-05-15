#pragma once

#include "RenderPass.h"



struct SShadowMapPass
{
	SShadowMapPass()
		: m_Renderer(nullptr)
		, m_ShadowMapShader(INDEX_NONE)
		, m_ShadowMapShaderInstanced(INDEX_NONE)
		, m_ShadowCubeMapShader(INDEX_NONE)
		, m_ShadowMapDimension_Spot(0)
		, m_ShadowMapDimension_Point(0)
		, m_ShadowMapTextures_Spot(INDEX_NONE)
		, m_ShadowMapTexture_Directional(INDEX_NONE)
		, m_ShadowMapTextures_Point(INDEX_NONE)
		, m_DepthTarget_Directional(INDEX_NONE)
		, m_ShadowCubeMapShaderInstanced(INDEX_NONE)
	{
	}

	void Initialize(class Renderer* pRenderer);

	void RenderShadowMaps(Renderer* pRenderer, const SShadowView& shadowView) ;

	SVector2 GetDirectionalShadowMapDimensions(Renderer* pRenderer) const;


	void InitializeSpotLightShadowMaps();
	void InitializePointLightShadowMaps();
	void InitializeDirectionalLightShadowMap();	



	Renderer*			m_Renderer;
	FShaderID			m_ShadowMapShader;
	FShaderID			m_ShadowMapShaderInstanced;
	FShaderID			m_ShadowCubeMapShader;
	FShaderID			m_ShadowCubeMapShaderInstanced;

	uint32				m_ShadowMapDimension_Spot;
	uint32				m_ShadowMapDimension_Point;
	uint32				m_ShadowMapDimension_Direction;

	FTextureID			m_ShadowMapTextures_Spot;		// tex2D array
	FTextureID			m_ShadowMapTexture_Directional;	// tex2D array
	FTextureID			m_ShadowMapTextures_Point;		// cubemap array

	FDepthTargetIDArray	m_DepthTargets_Spot;
	FDepthTargetID		m_DepthTarget_Directional;
	FDepthTargetIDArray	m_DepthTargets_Point;

};

