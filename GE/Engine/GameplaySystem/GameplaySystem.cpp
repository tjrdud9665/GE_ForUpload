#include "EnginePCH.h"
#include "GameplaySystem.h"
#include "../GameFramework/GameObject/GameObject.h"
#include "../GameFramework/Components/AbilityComponent.h"


void GameplaySystem::SendGameplayEvent(class GameObject* ToObject, const std::string& EventName, SGameplayEventData*& Data)
{
	IAbilitySystem* abiliySys = dynamic_cast<IAbilitySystem*>(ToObject);
	if (abiliySys)
	{
		AbilityComponent* AbilityComp = abiliySys->GetAbilityComponent();
		AbilityComp->HandleGameplayEvent(EventName, Data);
	}

	SAFE_DELETE(Data);
}

float GameplaySystem::GetAttributeValue(class GameObject* AttributeOwner, const std::string& AttributeName)
{
	IAbilitySystem* abiliySys = dynamic_cast<IAbilitySystem*>(AttributeOwner);
	if (abiliySys)
	{
		AbilityComponent* AbilityComp = abiliySys->GetAbilityComponent();
		return AbilityComp->GetAttribute(AttributeName);
	}

	return 0.0f;
}
