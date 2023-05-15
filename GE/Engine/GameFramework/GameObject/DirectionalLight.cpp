#include "EnginePCH.h"
#include "DirectionalLight.h"
#include "../Components/DirectionalLightComponent.h"

DirectionalLight::DirectionalLight()
{

}

DirectionalLight::DirectionalLight(const DirectionalLight& Other)
	:Super(Other)
	,m_DirectionLight(Other.m_DirectionLight)
{
	m_DirectionLight = FindComponent<DirectionalLightComponent>("DirectionalLight");
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_DirectionLight = CreateComponent<DirectionalLightComponent>("DirectionalLight");

	SetRootComponent(m_DirectionLight);	
}
