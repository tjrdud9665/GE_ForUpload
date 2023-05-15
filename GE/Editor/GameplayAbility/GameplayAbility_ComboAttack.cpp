#include "PCH.h"
#include "GameplayAbility_ComboAttack.h"
#include "ObjectManager.h"
#include "../GameObject/HitBoxObject.h"
#include <GameFramework/Components/SkeletalMeshComponent.h>

GameplayAbility_ComboAttack::GameplayAbility_ComboAttack()
	:m_ComboCount(0)
	, m_ComboEnable(false)
	, LastSpawnOffset(0.f)
{
}

GameplayAbility_ComboAttack::GameplayAbility_ComboAttack(const GameplayAbility_ComboAttack& Other)
	: Super(Other)
	, LastHitEffect(Other.LastHitEffect)
	, LastHitBoxClass(Other.LastHitBoxClass)
	, LastSpawnOffset(Other.LastSpawnOffset)
	, m_ComboCount(0)
	, m_ComboEnable(false)
{
}

GameplayAbility_ComboAttack::~GameplayAbility_ComboAttack()
{
}

void GameplayAbility_ComboAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (IsActivating())
	{
		// NextCombo Event가 들어왔을 시
		if ("NextCombo" == EventName)
		{
			if (m_ComboEnable)
			{
				int32 ComboCount = std::min((int32)(Attacks.size() - 1), ++m_ComboCount);


				if (ChasingMouse)
					ChangeDirection();

				// 다음 콤보 애니메이션 실행

				if (ChasingMouse)
					ChangeDirection();

				m_ObjectInfo.OwnerAvatar->SetPlayRate(AnimPlayRate);
				AbilityUtils::PlayAnimation(this, Attacks[ComboCount], false, RootMotion, true);

				// 초기화 해서 다시 재입력시 활성화
				m_ComboEnable = false;
			}


			// 여기서 EventName이 걸렸으면 끝
			return;
		}

		else if ("Spawn"== EventName)
		{
			// 마지막 콤보일 때 소환할 것
			if (m_ComboCount == (int32)(Attacks.size() - 1))
			{
				HitBoxObject* spawnedHitBox = nullptr;

				if (LastHitBoxClass)
					spawnedHitBox = g_Engine->Get<ObjectManager>()->CreateGameObject<HitBoxObject>(LastHitBoxClass);


				if (spawnedHitBox)
				{
					SVector3 Pos = m_ObjectInfo.OwnerObject->GetWorldLocation();
					SVector3 Dir = m_ObjectInfo.OwnerObject->GetObjectForwardVector();
					SVector3 Rot = m_ObjectInfo.OwnerObject->GetWorldRotation();

					spawnedHitBox->SetShooter(m_ObjectInfo.OwnerObject);
					spawnedHitBox->SetObjectLocation(Pos + Dir * LastSpawnOffset);
					spawnedHitBox->SetObjectRotation(SVector3(0.f, 0.f, Rot.y));

					g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(spawnedHitBox);
				}




				// 여기서 EventName이 걸렸으면 끝
				return;
			}
		}
	}


	// 위에서 EventName이 안걸렸으면 다시 조사
	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);
}

void GameplayAbility_ComboAttack::OnEndAbility()
{
	Super::OnEndAbility();

	m_ComboCount = 0;
	m_ComboEnable = false;
}

void GameplayAbility_ComboAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	// 재입력 시 콤보가 발동되도록 설정
	m_ComboEnable = true;
}
