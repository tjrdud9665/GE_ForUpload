#include "EnginePCH.h"
#include "LightComponent.h"
#include <Renderer/Renderer.h>

Matrix LightComponent::CalculateDirectionalLightViewMatrix(LightComponent* DirLight, const SVector3& CameraOffset)
{	

	const SVector3 up = SVector3::UpVector;
	const SVector3 lookAt = CameraOffset;
		//SVector3(0, 0, 15000);
		//SVector3::ZeroVector;

	const Matrix mRot = Matrix::StaticRotationQuaternion(DirLight->GetComponentTransform().Rotation);	
	const SVector3 direction = SVector3::ForwardVector.TransformNormal(mRot);		
	
	const SVector3 lightPos = direction * -1000.0f + lookAt;

	return XMMatrixLookAtLH(lightPos.Convert(), lookAt.Convert(), up.Convert());
}

Matrix LightComponent::CalculatePointLightViewMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir, const SVector3& position)
{
	return D3DTexture::CubemapUtility::CalculateViewMatrix(lookDir, position);

}

LightComponent::LightComponent()
	:m_Brightness(300.0f)
	, m_CastingShadow(false)
	,m_LightColor(SLinearColor(1,1,1,1))
{
	for(int32 i =0;i<6;i++)
	{
		m_ViewMatrix[i].Identity();
	}
}

LightComponent::LightComponent(const LightComponent& Other)
	: Super(Other)
	, m_Brightness(Other.m_Brightness)	
	, m_LightColor(Other.m_LightColor)
	, m_CastingShadow(Other.m_CastingShadow)
	, m_ViewMatrix(Other.m_ViewMatrix)
	, m_ProjectionMatrix(Other.m_ProjectionMatrix)
	,m_DepthBias(Other.m_DepthBias)
{
	for (int32 i = 0; i < 6; i++)
	{
		m_ViewMatrix[i] = Other.m_ViewMatrix[i];
	}
}

LightComponent::~LightComponent()
{

}

void LightComponent::GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights)
{

}

Matrix LightComponent::CalculateProjectionMatrix(SVector2 viewPortSize)
{
	return Matrix::StaticIdentity();
}

const Matrix& LightComponent::GetViewMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir /*= DEFAULT_POINT_LIGHT_LOOK_DIRECTION*/) const
{
	return m_ViewMatrix[0];
}

const Matrix& LightComponent::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}
