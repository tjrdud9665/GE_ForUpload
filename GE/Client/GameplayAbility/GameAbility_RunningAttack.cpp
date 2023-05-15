#include "PCH.h"
#include "GameAbility_RunningAttack.h"
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

GameAbility_RunningAttack::GameAbility_RunningAttack()
{

}

GameAbility_RunningAttack::GameAbility_RunningAttack(const GameAbility_RunningAttack& Other)
	:Super(Other)
	, m_MaxCount(Other.m_MaxCount)
	, m_KeyDownCheck(Other.m_KeyDownCheck)
	, m_RunningCount(Other.m_RunningCount)
{
}

GameAbility_RunningAttack::~GameAbility_RunningAttack()
{
}

void GameAbility_RunningAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
//	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);


	// Animation Notify���� 
	// Combo4 - 3 Animation
	// Notify RunningAttack  - 0.631
	// Notify RunningBack  - 0.405

	if (EventName == "RunningAttack")
	{
		if (m_KeyDownCheck && m_RunningCount < m_MaxCount)
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

			if (m_RunningCount >= m_MaxCount)
			{
				m_RunningCount = m_MaxCount;
			}			
		}
	}
}

void GameAbility_RunningAttack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	// ��Ÿ�ϸ鳪���°ŷ� ���� �ڿ��������ϱ����� �ִϸ��̼� ����ӵ� ������Ŵ .
	// �����Ƽ����� �ٽ� ����ӵ� ���� 

	m_ObjectInfo.OwnerAvatar->SetPlayRate(1.7f);

	AbilityUtils::PlayAnimation(this, Attack[0], false, false, true);
}

void GameAbility_RunningAttack::OnCancelAbility()
{
	m_ObjectInfo.OwnerAvatar->SetPlayRate(1.f);

	m_KeyDownCheck = false;
	m_KeepRunning = false;
	m_RunningCount = 0;
}

void GameAbility_RunningAttack::OnEndAbility()
{
	m_ObjectInfo.OwnerAvatar->SetPlayRate(1.f);	

	m_KeyDownCheck = false;
	m_KeepRunning = false;
	m_RunningCount = 0;

	m_ObjectInfo.OwnerAvatar->SetEnableRootMotion(false);
}

void GameAbility_RunningAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	m_KeyDownCheck = true;
}

void GameAbility_RunningAttack::InputReleased(const SGameplayAbilitySpec& InSpec)
{

}
