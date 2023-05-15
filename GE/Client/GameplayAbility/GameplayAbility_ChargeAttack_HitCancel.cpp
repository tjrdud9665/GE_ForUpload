#include "PCH.h"
#include "GameplayAbility_ChargeAttack_HitCancel.h"
#include "GameFramework/GameObject/GameObject.h"


GameplayAbility_ChargeAttack_HitCancel::GameplayAbility_ChargeAttack_HitCancel()
	: MaxHitCount(1)
	, m_HitCount(0)
{
}

GameplayAbility_ChargeAttack_HitCancel::GameplayAbility_ChargeAttack_HitCancel(const GameplayAbility_ChargeAttack_HitCancel& Other)
	: Super(Other)
	, MaxHitCount(Other.MaxHitCount)
	, m_HitCount(Other.m_HitCount)
{
}

GameplayAbility_ChargeAttack_HitCancel::~GameplayAbility_ChargeAttack_HitCancel()
{
}

void GameplayAbility_ChargeAttack_HitCancel::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (IsActivating())
	{
		if ("Hurt" == EventName)
		{
			++m_HitCount;

			if (m_HitCount >= MaxHitCount)
			{
				CancelAbility();
				InSpec.ObjectInfo.OwnerObject->AddTag("Groggy", true);
			}

			return;
		}
	}

	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);
}

void GameplayAbility_ChargeAttack_HitCancel::OnCancelAbility()
{
	Super::OnCancelAbility();

	m_HitCount = 0;
}

void GameplayAbility_ChargeAttack_HitCancel::OnEndAbility()
{
	Super::OnEndAbility();

	m_HitCount = 0;
}
