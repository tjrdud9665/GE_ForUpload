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
		// NextCombo Event�� ������ ��
		if ("NextCombo" == EventName)
		{
			if (m_ComboEnable)
			{
				int32 ComboCount = std::min((int32)(Attacks.size() - 1), ++m_ComboCount);


				if (ChasingMouse)
					ChangeDirection();

				// ���� �޺� �ִϸ��̼� ����

				if (ChasingMouse)
					ChangeDirection();

				m_ObjectInfo.OwnerAvatar->SetPlayRate(AnimPlayRate);
				AbilityUtils::PlayAnimation(this, Attacks[ComboCount], false, RootMotion, true);

				// �ʱ�ȭ �ؼ� �ٽ� ���Է½� Ȱ��ȭ
				m_ComboEnable = false;
			}


			// ���⼭ EventName�� �ɷ����� ��
			return;
		}

		else if ("Spawn"== EventName)
		{
			// ������ �޺��� �� ��ȯ�� ��
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




				// ���⼭ EventName�� �ɷ����� ��
				return;
			}
		}
	}


	// ������ EventName�� �Ȱɷ����� �ٽ� ����
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
	// ���Է� �� �޺��� �ߵ��ǵ��� ����
	m_ComboEnable = true;
}
