#pragma once
#include "GameObject.h"

class DirectionalLight :public GameObject
{
	CLASS(DirectionalLight,GameObject)

protected:
	DirectionalLight();
	DirectionalLight(const DirectionalLight& Other);
	virtual ~DirectionalLight();

	virtual void Construct(const std::string& InName /* = "" */) override;

private:
	class DirectionalLightComponent* m_DirectionLight;

	
};

