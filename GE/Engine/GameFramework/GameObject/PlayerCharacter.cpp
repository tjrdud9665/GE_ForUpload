#include "EnginePCH.h"
#include "PlayerCharacter.h"
#include "../Components/TargetArmComponent.h"
#include "../Components/CameraComponent.h"
#include <Assets/Scene/Scene.h>
#include <CameraManager.h>

PlayerCharacter::PlayerCharacter()
{
	
}

PlayerCharacter::PlayerCharacter(const PlayerCharacter& Other)
	:Super(Other)	
{
	m_CameraBoom = FindComponent<TargetArmComponent>("CameraBoom");
	m_FollowCamera = FindComponent<CameraComponent>("FoolowCamera");
}

PlayerCharacter::~PlayerCharacter()
{

}

void PlayerCharacter::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_CameraBoom			 = CreateComponent<TargetArmComponent>("CameraBoom");
	m_FollowCamera			 = CreateComponent<CameraComponent>("FoolowCamera");
	
	m_CameraBoom->AddChild(m_FollowCamera);
	GetRootComponent()->AddChild(m_CameraBoom);

	 
}

void PlayerCharacter::PossessedBy(class Controller* NewController)
{
	Super::PossessedBy(NewController);

	GetOwningScene()->GetCameraManager()->SetCurrentCamera(m_FollowCamera);
		
}
