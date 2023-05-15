#pragma once
#include <GameFramework/GameObject/GameObject.h>

class ParticleObject : public GameObject
{
	CLASS(ParticleObject, GameObject)

protected:
	ParticleObject();
	ParticleObject(const ParticleObject& Other);
	virtual ~ParticleObject();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PreUpdate(float DeltaSeconds)		override;

	virtual void Update(float DeltaSeconds)			override;

	virtual void PostUpdate(float DeltaSeconds)		override;

public:
	virtual void BeginPlay() override;

protected:
	TSharedPtr<class ParticleComponent>	m_Particle;

	PROP(float, m_DeadTime, DetailsEdit)
		float	m_DeadTime;

	float m_DeadAccTime;
};

