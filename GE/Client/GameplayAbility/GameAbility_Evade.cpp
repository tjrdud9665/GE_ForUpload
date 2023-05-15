#include "PCH.h"
#include "GameAbility_Evade.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include "ObjectManager.h"
#include "../GameObject/HitBoxObject.h"
#include "GameplaySystem/GameplaySystem.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/GameObject/Character.h"
#include "Input/Input.h"
#include "Assets/GameplayAbility/GameplayAbility.h"

GameAbility_Evade::GameAbility_Evade()
{

}

GameAbility_Evade::GameAbility_Evade(const GameAbility_Evade& Other)
	:Super(Other)
{
	size_t Size = Other.EvadeAnims.size();

	EvadeAnims.resize(Size);

	for (int i = 0; i < Size; ++i)
	{
		EvadeAnims[i] = Other.EvadeAnims[i];
	}
}

GameAbility_Evade::~GameAbility_Evade()
{

}

void GameAbility_Evade::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);

}

void GameAbility_Evade::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{	
	AbilityUtils::PlayAnimation(this, EvadeAnims[0], false, true, true);
}

void GameAbility_Evade::OnCancelAbility()
{

}

void GameAbility_Evade::OnEndAbility()
{
	m_ObjectInfo.OwnerAvatar->SetEnableRootMotion(false);
}

void GameAbility_Evade::InputPressed(const SGameplayAbilitySpec& InSpec)
{

}

void GameAbility_Evade::InputReleased(const SGameplayAbilitySpec& InSpec)
{

}
