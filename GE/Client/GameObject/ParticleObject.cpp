#include "PCH.h"
#include "ParticleObject.h"
#include "GameFramework/Components/PrimitiveComponent.h"
#include "GameFramework/Components/ParticleComponent.h"

ParticleObject::ParticleObject()
	: m_DeadTime(0.f)
	, m_DeadAccTime(0.f)
{

}

ParticleObject::ParticleObject(const ParticleObject& Other)
	: Super(Other)
	, m_DeadAccTime(Other.m_DeadAccTime)
	, m_DeadTime(Other.m_DeadTime)
{
	m_Particle = FindComponent<ParticleComponent>("Particle");
}

ParticleObject::~ParticleObject()
{

}

void ParticleObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_Particle = CreateComponent<ParticleComponent>("Particle");

	SetRootComponent(m_Particle);
}

void ParticleObject::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
}

void ParticleObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void ParticleObject::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	if (m_Particle->IsPlaying())
	{
		m_DeadAccTime += DeltaSeconds;

		if (m_DeadAccTime >= m_DeadTime)
		{
			Destroy();
		}
	}
}

void ParticleObject::BeginPlay()
{
	Super::BeginPlay();
}