#include "EnginePCH.h"
#include "ComponentBase.h"

ComponentBase::ComponentBase()
	:m_OwningScene(nullptr)
	, m_LifeTime(0.0f)
	, m_DestroyTimerID(0)
	, m_IsBegun(false)
{

}

ComponentBase::ComponentBase(const ComponentBase& Other)
	:Super(Other)
	, m_LifeTime(Other.m_LifeTime)
	, m_DestroyTimerID(0)
	, m_IsBegun(Other.m_IsBegun)
{

}

ComponentBase::~ComponentBase()
{
	
}

void ComponentBase::BeginPlay()
{
	m_IsBegun = true;
}

void ComponentBase::PreUpdate(float DeltaSeconds)
{

}

void ComponentBase::Update(float DeltaSeconds)
{

}

void ComponentBase::PostUpdate(float DeltaSeconds)
{

}

void ComponentBase::EndPlay()
{
	m_IsBegun = false;
}

void ComponentBase::SetLifeTime(const float InLifeTime)
{
	if (InLifeTime <= 0)
	{
		Kill();
		return;
	}

		
	m_TimerHandle.RemoveTimer(OUT m_DestroyTimerID);

	m_LifeTime = InLifeTime;
	m_TimerHandle.SetTimer(OUT m_DestroyTimerID, m_LifeTime, false, std::bind(&ComponentBase::Kill, this));
}

void ComponentBase::Destroy(float TimeDelay)
{
	if (TimeDelay > 0)
	{
		SetLifeTime(TimeDelay);
	} 
	else
	{
		Kill();
	}
}

void ComponentBase::SetOwningScene(class Scene* InScene)
{
	m_OwningScene = InScene;
}

class Scene* ComponentBase::GetOwningScene() const
{
	return m_OwningScene;
}

void ComponentBase::OnDestroy()
{
	Super::OnDestroy();
	m_TimerHandle.RemoveTimer(m_DestroyTimerID);	
}


bool ComponentBase::IsPlaying() const
{
	return m_IsBegun;
}