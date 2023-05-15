#include "PCH.h"
#include "GameplayAbility_ChargeAttack.h"
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


GameplayAbility_ChargeAttack::GameplayAbility_ChargeAttack()
	: MaxChargingTime(1.f)
	, m_ChargeStartTime(0.0f)
	, m_ChargeState(false)
	, m_ChargeLevel(0)
{

}

GameplayAbility_ChargeAttack::GameplayAbility_ChargeAttack(const GameplayAbility_ChargeAttack& Other)
	: Super(Other)
	, ChargeHitEffect(Other.ChargeHitEffect)
	, MaxChargingTime(Other.MaxChargingTime)
	, m_ChargeStartTime(0.0f)
	, m_ChargeState(false)
	, m_ChargeLevel(0)
	, m_ChargeTimer(Other.m_ChargeTimer)
	, m_ChargeMaxTimerID(Other.m_ChargeMaxTimerID)
	, m_ChargeLevelTimerID(Other.m_ChargeLevelTimerID)
{
}

GameplayAbility_ChargeAttack::~GameplayAbility_ChargeAttack()
{

}

void GameplayAbility_ChargeAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (IsActivating())
	{
		if ("ChargeStart" == EventName)
		{
			// 애니메이션 중에 Charge를 시작하는 부분을 체크해서 비율을 저장해둠
			if (m_ObjectInfo.OwnerAvatar)
			{
				if (m_ChargeStartTime != 0.0f)
					m_ChargeStartTime = m_ObjectInfo.OwnerAvatar->GetPlaybackPosition();
			}

			return;
		}

		else if ("Charging" == EventName)
		{
			// 차징 상태라면 저장해둔 비율로 애니메이션을 돌려줌
			if (m_ChargeState)
			{
				if (m_ObjectInfo.OwnerAvatar)
				{
					m_ObjectInfo.OwnerAvatar->SetPlaybackPosition(m_ChargeStartTime);

					// 호출된 노티파이도 리셋 시켜줘야함.
					m_ObjectInfo.OwnerAvatar->ResetAllNotifies();
				}
			}

			return;
		}

		else if ("KnockDown" == EventName)
		{
			// 풀차징 됐을 때는 Effect 변경
			if (m_ChargeLevel == (int32)MaxChargingTime)
			{
				if (!EventData)
					return;

				IAbilitySystem* abilitySys = dynamic_cast<IAbilitySystem*>(EventData->Target);

				if (abilitySys)
				{

					AbilityComponent* targetComp = abilitySys->GetAbilityComponent();
					AbilityComponent* ShooterComp = m_ObjectInfo.OwnerComponent;

					targetComp->GetOwner<Character>()->AddTag("KnockDown", true);

					ShooterComp->ApplyGameplayEffectToTarget(ChargeHitEffect, targetComp);

				}

				return;

			}
		}

		else if ("Hit" == EventName)
		{
			// 풀차징 됐을 때는 Effect 변경
			if (m_ChargeLevel == (int32)MaxChargingTime)
			{
				if (!EventData)
					return;

				IAbilitySystem* abilitySys = dynamic_cast<IAbilitySystem*>(EventData->Target);

				if (abilitySys)
				{

					AbilityComponent* targetComp = abilitySys->GetAbilityComponent();
					AbilityComponent* ShooterComp = m_ObjectInfo.OwnerComponent;

					ShooterComp->ApplyGameplayEffectToTarget(ChargeHitEffect, targetComp);

				}

				return;

			}
		}
	}


	// 위에서 EventName이 안걸렸으면 다시 조사
	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);
}

void GameplayAbility_ChargeAttack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	//m_ObjectInfo.OwnerObject->AddTag("CantMove", true);
	m_ObjectInfo.OwnerObject->AddTag("CanAbility", false);

	if (ChasingMouse)
		ChangeDirection();

	m_ChargeState = true;

	if (ChasingMouse)
		ChangeDirection();

	m_ObjectInfo.OwnerAvatar->SetPlayRate(AnimPlayRate);
	AbilityUtils::PlayAnimation(this, Attacks[0], false, RootMotion, true);

	m_ChargeTimer.SetTimer(OUT m_ChargeMaxTimerID, MaxChargingTime + 0.1f, false, std::bind(&GameplayAbility_ChargeAttack::MaxChargeTime, this));
	m_ChargeTimer.SetTimer(OUT m_ChargeLevelTimerID, 1.f, true, std::bind(&GameplayAbility_ChargeAttack::AddChargeLevel, this));
}

void GameplayAbility_ChargeAttack::OnCancelAbility()
{
	Super::OnCancelAbility();

	m_ChargeState = false;
	m_ChargeStartTime = 0.0f;
	m_ChargeLevel = 0;

	m_ChargeTimer.RemoveTimer(m_ChargeMaxTimerID);
	m_ChargeTimer.RemoveTimer(m_ChargeLevelTimerID);
}

void GameplayAbility_ChargeAttack::OnEndAbility()
{
	Super::OnEndAbility();

	// 어빌리티 종료시 멤버 리셋 
	m_ChargeState = false;
	m_ChargeStartTime = 0.0f;
	m_ChargeLevel = 0;

	m_ChargeTimer.RemoveTimer(m_ChargeMaxTimerID);
	m_ChargeTimer.RemoveTimer(m_ChargeLevelTimerID);
}

void GameplayAbility_ChargeAttack::InputReleased(const SGameplayAbilitySpec& InSpec)
{
	// 키를 뗏을 경우 
	m_ChargeState = false;
}

void GameplayAbility_ChargeAttack::MaxChargeTime()
{
	m_ChargeTimer.RemoveTimer(m_ChargeLevelTimerID);

	m_ChargeState = false;
}

void GameplayAbility_ChargeAttack::AddChargeLevel()
{
	// 1초마다 해당함수 실행 , Charge단계 증가 , 추후 데미지 계산을 위함

	if (m_ChargeState)
		++m_ChargeLevel;
}
