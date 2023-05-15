#include "EnginePCH.h"
#include "PlayerController.h"
#include "Character.h"

PlayerController::PlayerController()
{

}

PlayerController::PlayerController(const PlayerController& Other)
	:Super(Other)
{

}

PlayerController::~PlayerController()
{

}

void PlayerController::OnPossess(class Character* InCharacter)
{
	Super::OnPossess(InCharacter);

	//m_InputHandle
	InCharacter->SetUpInput(m_CharacterInputHandle);//
}

void PlayerController::OnUnPossess()
{	
	Super::OnUnPossess();

	m_CharacterInputHandle.ClearAllBinds();
}
