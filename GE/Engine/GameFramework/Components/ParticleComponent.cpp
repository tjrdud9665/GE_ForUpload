#include "EnginePCH.h"
#include "ParticleComponent.h"

ParticleComponent::ParticleComponent()
	:m_ParticleTimeAcc(0.0f)
	, m_ParticleArray(INDEX_NONE)
	, m_ParticleShare(INDEX_NONE)
{
	
}

ParticleComponent::ParticleComponent(const ParticleComponent& Other)
	:Super(Other)
	, m_Particle(Other.m_Particle)
	, m_ParticleTimeAcc(0.0f)
	, m_ParticleArray(INDEX_NONE)
	, m_ParticleShare(INDEX_NONE)
{
	m_CachedRenderer = g_Engine->Get<Renderer>();
}

ParticleComponent::~ParticleComponent()
{

}

void ParticleComponent::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_CachedRenderer = g_Engine->Get<Renderer>();
}

void ParticleComponent::Render(class Renderer* pRenderer)
{
	if (!m_Particle)
		return;

	CameraComponent* currentCamera = nullptr;
	if (m_OwningScene)
		currentCamera = m_OwningScene->GetCameraManager()->GetCurrentCamera();
	else
		currentCamera = g_Engine->Get<CameraManager>()->GetCurrentCamera();

	Matrix matProj = currentCamera->GetProjMatrix();
	Matrix matView = currentCamera->GetViewMatrix();	
	
	Matrix vp = matView * matProj;

	pRenderer->SetConstant4x4f("ViewProj", vp);		
	pRenderer->SetConstant4f("CameraPostion", SVector4(currentCamera->GetWorldLocation(), 1.0f));
	//Super::Render(pRenderer);
	
	if (m_ParticleArray != INDEX_NONE)
		pRenderer->SetStructured("ComputedParticleArray", m_ParticleArray);

	if (m_ParticleShare != INDEX_NONE)
		pRenderer->SetStructured("ComputedParticleShare", m_ParticleShare);

	if(m_Particle)
	{
		m_Particle->BindRenderingContext(pRenderer);
	}	
	
	pRenderer->Apply();
	pRenderer->DrawInstanced(1,m_Particle->SpawnCountMax);
}


void ParticleComponent::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);	
	
	m_ParticleTimeAcc += DeltaSeconds;
	

	ComputeParticles(m_CachedRenderer,DeltaSeconds);
}

void ParticleComponent::ComputeParticles(class Renderer* pRenderer, float DeltaSeconds)
{
	if(m_Particle)
	{
		pRenderer->SetShader(EShaders::PARTICLE_UPDATE);
		SParticleUpdateInfo particleInfo;
		particleInfo.SpawnEnable = 1;
		particleInfo.KillAllParticle = 0;

		m_Particle->MakeParticleInfo(OUT particleInfo);		

		pRenderer->SetConstantStruct("particleInfo", &particleInfo);
		pRenderer->SetConstant1f("TimeAcc", m_ParticleTimeAcc);
		pRenderer->SetConstant1f("DeltaTime", DeltaSeconds);
		pRenderer->SetConstant4f("WorldOffset",SVector4(GetWorldLocation(), 1.0f));
		
		if (m_ParticleArray == INDEX_NONE)
			m_ParticleArray = pRenderer->CreateRWStructured("ParticleArray", EShaders::PARTICLE_UPDATE, nullptr,particleInfo.SpawnCountMax);

		if (m_ParticleShare == INDEX_NONE)
			m_ParticleShare = pRenderer->CreateRWStructured("ParticleShare", EShaders::PARTICLE_UPDATE, nullptr, 1);

		
		pRenderer->SetRWStructured(m_ParticleArray, particleInfo.SpawnCountMax);
		pRenderer->SetRWStructured(m_ParticleShare, 1);
		pRenderer->Apply();
		uint32 GroupX = particleInfo.SpawnCountMax / 64 + 1;
		pRenderer->Dispatch(GroupX, 1, 1);
		pRenderer->ClearSRV();
	}
}

void ParticleComponent::SetParticle(ParticleAsset* InParticle)
{
	m_Particle = InParticle;
}
