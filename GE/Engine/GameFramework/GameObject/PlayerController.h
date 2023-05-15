#pragma once
#include "Controller.h"
#include <Input/InputHandle.h>
class PlayerController :public Controller
{
	CLASS(PlayerController,Controller)

protected:
	PlayerController();
	PlayerController(const PlayerController& Other);
	virtual ~PlayerController();

public:
	virtual void OnPossess(class Character* InCharacter) override;

	virtual void OnUnPossess() override;

	const InputHandle& GetControllerInputHandle() const;

	const InputHandle& GetCharacterInputHandle() const;

protected:
	InputHandle		m_CharacterInputHandle;

	InputHandle		m_ControllerInputHandle;

};

