#pragma once
#include "LightComponent.h"

/*
* 
*/
class DirectionalLightComponent : public LightComponent
{
	CLASS(DirectionalLightComponent, LightComponent)

protected:
	DirectionalLightComponent();
	DirectionalLightComponent(const DirectionalLightComponent& Other);
	virtual ~DirectionalLightComponent();


	virtual void OnUpdateTransform() override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual Matrix CalculateProjectionMatrix(SVector2 viewPortSize);

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void BeginPlay() override;

	void UpdateViewTransform(class CameraManager* Mgr);


public:	
	virtual void GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights) override;

private:
	SVector3	m_CameraOffset;


};

