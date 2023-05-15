#pragma once

#include "SceneComponent.h"
#include "Assets/Particle/ParticleAsset.h"

class ParticleComponent :public SceneComponent
{
	CLASS(ParticleComponent,SceneComponent)

protected:
	ParticleComponent();
	ParticleComponent(const ParticleComponent& Other);
	virtual ~ParticleComponent();

public:
	virtual void Construct(const std::string& InName = "") override;

	virtual void Render(class Renderer* pRenderer) override;

	virtual void Update(float DeltaSeconds) override;

	void ComputeParticles(class Renderer* pRenderer,float DeltaSeconds);

	void SetParticle(ParticleAsset* InParticle);

private:
	PROP(TSharedPtr<ParticleAsset>, m_Particle, DetailsEdit)
	TSharedPtr<ParticleAsset> m_Particle;

	class Renderer* m_CachedRenderer;

	float			m_ParticleTimeAcc;

	FBufferID			m_ParticleArray;

	FBufferID			m_ParticleShare;

};

