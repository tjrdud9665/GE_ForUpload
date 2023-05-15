#pragma once
#include "GameObject.h"

class Controller : public GameObject
{
	CLASS(Controller,GameObject)

protected:
	Controller();
	Controller(const Controller& Other);
	virtual ~Controller();

public:
	virtual void Construct(const std::string& InName = "") override;

public:
	void Possess(class Character* InCharacter);
	void UnPossess();



	class Character* GetCharacter() const;
	void SetCharacter(class Character* InCharacter);

protected:
	virtual void OnPossess(class Character* InCharacter);
	virtual void OnUnPossess();

	
//private:
	//TSharedPtr<class SceneComponent>		m_Transform;

protected:
	class Character* m_Character;
	std::function<void(class Character*)>	m_OnNewCharacter;
};

