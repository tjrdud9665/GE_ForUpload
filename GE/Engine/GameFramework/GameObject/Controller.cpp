#include "EnginePCH.h"
#include "Controller.h"
#include "Character.h"

Controller::Controller()
{

}

Controller::Controller(const Controller& Other)
	:Super(Other)
{

}

Controller::~Controller()
{

}

void Controller::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
}

void Controller::Possess(class Character* InCharacter)
{
	const Character* CurrentCharacter = GetCharacter();
	const bool NotificationRequired = (CurrentCharacter != nullptr && CurrentCharacter->GetController() == nullptr);

	OnPossess(InCharacter);

	Character* NewChar = GetCharacter();
	if (NewChar != CurrentCharacter || NotificationRequired)
	{
		if (m_OnNewCharacter)
			m_OnNewCharacter(NewChar);
	}
}

void Controller::UnPossess()
{
	Character* CurrentChar = GetCharacter();


	if (CurrentChar == nullptr)
	{
		return;
	}

	OnUnPossess();


	Character* NewChar = GetCharacter();
	if (NewChar != CurrentChar)
	{
		if (m_OnNewCharacter)
			m_OnNewCharacter(NewChar);
	}
}

void Controller::OnPossess(class Character* InCharacter)
{
	const bool IsNewChar = GetCharacter() != InCharacter;


	if (IsNewChar && GetCharacter() != nullptr)
	{
		UnPossess();
	}

	if (InCharacter == nullptr)
	{
		return;
	}

	if (InCharacter->m_Controller != nullptr)
	{

		InCharacter->m_Controller->UnPossess();
	}

	InCharacter->PossessedBy(this);
	SetCharacter(InCharacter);
}

void Controller::OnUnPossess()
{
	if (m_Character != nullptr)
	{
		m_Character->UnPossessed();
		SetCharacter(nullptr);
	}
}

class Character* Controller::GetCharacter() const
{
	return m_Character;
}

void Controller::SetCharacter(class Character* InCharacter)
{
	m_Character = InCharacter;
}
