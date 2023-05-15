#pragma once
#include "GameFramework\GameObject\GameObject.h"

class PointLightObject : public GameObject
{
	CLASS(PointLightObject, GameObject)

protected:
	PointLightObject();
	PointLightObject(const PointLightObject& Other);
	virtual ~PointLightObject();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PreUpdate(float DeltaSeconds)		override;

	virtual void Update(float DeltaSeconds)			override;

	virtual void PostUpdate(float DeltaSeconds)		override;

public:
	virtual void BeginPlay() override;

protected:
	TSharedPtr<class PointLightComponent>	m_PointLight;

};

