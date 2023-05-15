#pragma once
#include "LightComponent.h"

class SpotLightComponent :public LightComponent
{
	CLASS(SpotLightComponent, LightComponent)

protected:
	SpotLightComponent();
	SpotLightComponent(const SpotLightComponent& Other);
	virtual ~SpotLightComponent();

public:
	virtual void GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights) override;
	

	float GetOuterConeAngle() const
	{
		return OuterConeAngle;
	}

	float GetInnerConeAngle() const
	{
		return InnerConeAngle;
	}

private:
	PROP(float, OuterConeAngle, DetailsEdit, "Light")
	float			OuterConeAngle;

	PROP(float, InnerConeAngle, DetailsEdit, "Light")
	float			InnerConeAngle;
};

