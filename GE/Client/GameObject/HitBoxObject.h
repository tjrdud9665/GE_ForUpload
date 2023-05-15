#pragma once
#include "GameFramework\GameObject\GameObject.h"

class HitBoxObject : public GameObject
{
	CLASS(HitBoxObject, GameObject)

protected:
	HitBoxObject();
	HitBoxObject(const HitBoxObject& Other);
	virtual ~HitBoxObject();

	virtual void Construct(const std::string& InName /* = "" */) override;

public:
	virtual void BeginPlay() override;

public:
	void SetShooter(class GameObject* InShooter);

protected:
	TSharedPtr<class BoxComponent>	m_Box;

	uint64				m_DeadTimerID;;

	PROP(float, m_DeadTime, DetailsEdit)
	float	m_DeadTime;

	PROP(std::string, m_HitEventName, DetailsEdit)
	std::string	m_HitEventName;

	class GameObject* m_Shooter;

private:
	void CollisionOn(const SCollisionResult& Result);
};

