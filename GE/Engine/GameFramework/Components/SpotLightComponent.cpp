#include "EnginePCH.h"
#include "SpotLightComponent.h"

SpotLightComponent::SpotLightComponent()
	:OuterConeAngle(80.0f)
	,InnerConeAngle(80.0f)
{

}

SpotLightComponent::SpotLightComponent(const SpotLightComponent& Other)
	:Super(Other)
	, InnerConeAngle(Other.InnerConeAngle)
	, OuterConeAngle(Other.OuterConeAngle)
{

}

SpotLightComponent::~SpotLightComponent()
{

}



void SpotLightComponent::GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights)
{

	SSpotLight spotLight;
	spotLight.brightness = m_Brightness;
	spotLight.color = m_LightColor.ToVector3();
	spotLight.position = GetWorldLocation();

	SVector3 Rotator = GetWorldRotation();
	SVector3 direction = SVector3::ForwardVector;
	direction = direction.TransformCoord(Matrix::StaticRotation(Rotator));
	spotLight.spotDir = direction;
	spotLight.innerConeAngle = SMathUtils::DegreesToRadians(InnerConeAngle);
	spotLight.halfAngle = SMathUtils::DegreesToRadians(OuterConeAngle);
	spotLight.depthBias = m_DepthBias;

	SpotLights[OutSceneLighting.numSpots++] = spotLight;

	if (m_CastingShadow)
		OutSceneLighting.numSpotCasters++;
}
