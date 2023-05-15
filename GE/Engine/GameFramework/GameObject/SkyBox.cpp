#include "EnginePCH.h"
#include "SkyBox.h"
#include "../Components/SkyComponent.h"

SkyBox::SkyBox()
{

}

SkyBox::SkyBox(const SkyBox& Other)
	:Super(Other)
{
	m_SkyComponent = FindComponent<SkyComponent>("SkyComponent");
}

SkyBox::~SkyBox()
{

}

void SkyBox::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_SkyComponent = CreateComponent<SkyComponent>("SkyComponent");



}
