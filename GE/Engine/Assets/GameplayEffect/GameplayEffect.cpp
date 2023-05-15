#include "EnginePCH.h"
#include "GameplayEffect.h"


GameplayEffect::GameplayEffect()
	:DurationPolicy(EGameplayEffectDurationType::Instant)
	, DurationMagnitude(0.0f)
	, Period(0.0f)	
{

}

GameplayEffect::GameplayEffect(const GameplayEffect& Other)
	:Super(Other)
	, DurationPolicy(Other.DurationPolicy)
	, DurationMagnitude(Other.DurationMagnitude)
	, Period(Other.Period)	
{
	size_t modSize = Other.AttributeModifiers.size();
	AttributeModifiers.resize(modSize);

	for (size_t i = 0; i < modSize; i++)
	{
		AttributeModifiers[i] = Other.AttributeModifiers[i]->Clone();
	}
}

GameplayEffect::~GameplayEffect()
{
	if (!m_OwningScene)
	{
		std::string sPath = GetInfo().GetSPath();
		if (!sPath.empty())
		{
			std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(sPath)) + GetName();
			GetClass()->DeleteCDO(CDOName);
		}	
	}

	size_t modSize = AttributeModifiers.size();
	for (size_t i = 0; i < modSize; i++)
	{
		SAFE_DELETE(AttributeModifiers[i]);
	}
	AttributeModifiers.clear();
}

bool GameplayEffect::PostLoad()
{
	bool Result = Super::PostLoad();

	std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(GetInfo().GetSPath())) + GetName();

	GetClass()->RegisterCDO(CDOName, this, ABILITYSYSTEM_PATH);


	return Result;
}

void GameplayEffect::SetOwner(XObject* InOwner)
{
	Super::SetOwner(InOwner);

	if (InOwner->GetClass()->IsChildOf(AbilityComponent::StaticClass()) ||
		InOwner->GetClass()->IsA(AbilityComponent::StaticClass()))
	{
		m_OwningScene = ((AbilityComponent*)InOwner)->GetOwningScene();
	}
}

