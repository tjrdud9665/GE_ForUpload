#include "PCH.h"
#include "GameAbillity_RunningAttack.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include "ObjectManager.h"
#include "GameFramework/GameObject/HitBoxObject.h"
#include "GameplaySystem/GameplaySystem.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/GameObject/Character.h"
#include "Input/Input.h"
#include "Assets/GameplayAbility/GameplayAbility.h"

GameAbillity_RunningAttack::GameAbillity_RunningAttack()
{

}

GameAbillity_RunningAttack::GameAbillity_RunningAttack(const GameAbillity_RunningAttack& Other)
	:Super(Other)
{

}

GameAbillity_RunningAttack::~GameAbillity_RunningAttack()
{

}

void GameAbillity_RunningAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);

	if (EventName == "RunningAttack")
	{
		if (m_KeyDownCheck)
		{
			m_KeyDownCheck = false;

			m_ObjectInfo.OwnerAvatar->Reverse();

			m_KeepRunning = true;
		}
	}

	if (EventName == "RunningBack")
	{
		if (m_KeepRunning)
		{
			m_ObjectInfo.OwnerAvatar->Play();

			m_KeepRunning = false;
		}

	}

}

void GameAbillity_RunningAttack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	m_ObjectInfo.OwnerAvatar->SetPlayRate(1.7f);

	AbilityUtils::PlayAnimation(this, Attack[0], false, true, true);
}

void GameAbillity_RunningAttack::OnCancelAbility()
{

}

void GameAbillity_RunningAttack::OnEndAbility()
{
	
}

void GameAbillity_RunningAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	m_KeyDownCheck = true;
}

void GameAbillity_RunningAttack::InputReleased(const SGameplayAbilitySpec& InSpec)
{

}
