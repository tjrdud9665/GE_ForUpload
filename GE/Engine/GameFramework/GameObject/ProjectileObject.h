#pragma once

#include "GameObject.h"

class ProjectileObject :public GameObject
{

	CLASS(ProjectileObject, GameObject)

protected:
	ProjectileObject();
	ProjectileObject(const ProjectileObject& Other);
	virtual ~ProjectileObject();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PreUpdate(float DeltaSeconds)		override;

	virtual void Update(float DeltaSeconds)			override;

	virtual void PostUpdate(float DeltaSeconds)		override;

protected:
	TSharedPtr<class BoxComponent>	m_Box;
	float							m_DeadTime;


};

