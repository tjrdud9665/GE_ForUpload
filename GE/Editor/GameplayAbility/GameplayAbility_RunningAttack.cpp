#include "PCH.h"
#include "GameplayAbility_RunningAttack.h"
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

GameplayAbility_RunningAttack::GameplayAbility_RunningAttack()
{

}

GameplayAbility_RunningAttack::GameplayAbility_RunningAttack(const GameplayAbility_RunningAttack& Other)
	:Super(Other)
	, MaxCount(Other.MaxCount)
	, m_KeyDownCheck(Other.m_KeyDownCheck)
	, m_KeepRunning(false)
	, m_RunningCount(Other.m_RunningCount)
{
}

GameplayAbility_RunningAttack::~GameplayAbility_RunningAttack()
{
}

void GameplayAbility_RunningAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	//	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);


		// Animation Notify¡§∫∏ 
		// Combo4 - 3 Animation
		// Notify RunningAttack  - 0.631
		// Notify RunningBack  - 0.405

	if (EventName == "RunningAttack")
	{
		if (m_KeyDownCheck && m_RunningCount < MaxCount)
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

			++m_RunningCount;

			if (m_RunningCount >= MaxCount)
			{
				m_RunningCount = MaxCount;
			}
		}
	}
}

void GameplayAbility_RunningAttack::OnCancelAbility()
{
	Super::OnCancelAbility();

	m_KeyDownCheck = false;
	m_KeepRunning = false;
	m_RunningCount = 0;
}

void GameplayAbility_RunningAttack::OnEndAbility()
{
	Super::OnEndAbility();

	m_KeyDownCheck = false;
	m_KeepRunning = false;
	m_RunningCount = 0;

	m_ObjectInfo.OwnerAvatar->SetEnableRootMotion(false);
}

void GameplayAbility_RunningAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	m_KeyDownCheck = true;
}
