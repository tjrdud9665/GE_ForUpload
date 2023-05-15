#pragma once
#include "SceneComponent.h"
#include <Renderer/D3DTexture.h>

static float Near = 0.15f;
static float Far = 100000000.0f;

#define DEFAULT_POINT_LIGHT_LOOK_DIRECTION D3DTexture::CubemapUtility::ECubeMapLookDirections::CUBEMAP_LOOK_FRONT

class LightComponent :public SceneComponent
{
	CLASS(LightComponent,SceneComponent)

protected:
	static Matrix CalculateDirectionalLightViewMatrix(LightComponent* DirLight,const SVector3& CameraOffset);
	//static Matrix CalculateSpotLightViewMatrix(const Transform& mTransform);
	static Matrix CalculatePointLightViewMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir, const SVector3& position);


protected:
	LightComponent();
	LightComponent(const LightComponent& Other);
	virtual ~LightComponent();


public:	

	//Matrix	GetProjectionMatrix() const;
	virtual void GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights) ;

	bool IsCastingShadow() const
	{
		return m_CastingShadow;
	}

	virtual Matrix CalculateProjectionMatrix(SVector2 viewPortSize);

	virtual const Matrix& GetViewMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir = DEFAULT_POINT_LIGHT_LOOK_DIRECTION) const;

	virtual Matrix GetLightSpaceMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir = DEFAULT_POINT_LIGHT_LOOK_DIRECTION) const
	{
		return GetViewMatrix(lookDir) * GetProjectionMatrix();
	}

	const Matrix& GetProjectionMatrix() const;


	virtual float GetLightRange() const
	{
		return 0.0f;
	}

protected:
	//Dir / Spot / Point
	PROP(SLinearColor,m_LightColor,DetailsEdit,"Light")
	SLinearColor	m_LightColor;

	PROP(float, m_Brightness, DetailsEdit, "Light")
	float			m_Brightness;

	PROP(bool, m_CastingShadow, DetailsEdit, "Light")
	bool			m_CastingShadow;		
	
	PROP(float, m_DepthBias, DetailsEdit, "Light")
	float			m_DepthBias;

	Matrix		m_ProjectionMatrix;
	std::array<Matrix, 6> m_ViewMatrix;



};

