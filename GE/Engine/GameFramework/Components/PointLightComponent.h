#pragma once
#include "LightComponent.h"

class PointLightComponent :public LightComponent
{
	CLASS(PointLightComponent, LightComponent)

protected:
	PointLightComponent();
	PointLightComponent(const PointLightComponent& Other);
	virtual ~PointLightComponent();

public:
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void OnUpdateTransform() override;

	virtual void GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights,  std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights) override;

	virtual float GetLightRange() const override
	{
		return Range;
	}
	virtual const Matrix& GetViewMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir = DEFAULT_POINT_LIGHT_LOOK_DIRECTION) const;

	virtual Matrix CalculateProjectionMatrix(SVector2 viewPortSize) override;

private:
	PROP(float, Range, DetailsEdit, "Light")
	float			Range;
};

