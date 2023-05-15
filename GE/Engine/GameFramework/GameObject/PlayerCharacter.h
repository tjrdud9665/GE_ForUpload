#pragma once
#include "Character.h"

class PlayerCharacter :public Character
{
	CLASS(PlayerCharacter,Character)

protected:
	PlayerCharacter();
	PlayerCharacter(const PlayerCharacter& Other);
	virtual ~PlayerCharacter();


	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PossessedBy(class Controller* NewController) override;

private:
	class TargetArmComponent*		m_CameraBoom;

	class CameraComponent*			m_FollowCamera;
};

