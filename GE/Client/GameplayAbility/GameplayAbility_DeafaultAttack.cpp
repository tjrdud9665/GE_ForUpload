#include "PCH.h"
#include "GameplayAbility_DeafaultAttack.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/GameObject/GameObject.h>

GameplayAbility_DeafaultAttack::GameplayAbility_DeafaultAttack()
	: m_ComboCount(0)
	, m_ComboEnable(false)
{
}

GameplayAbility_DeafaultAttack::GameplayAbility_DeafaultAttack(const GameplayAbility_DeafaultAttack& Other)
	: Super(Other)
	, m_ComboCount(0)
	, m_ComboEnable(false)
{
	size_t ComboCount = Other.Combos.size();
	Combos.resize(ComboCount);

	for (size_t i = 0; i < ComboCount; ++i)
	{
		Combos[i] = Other.Combos[i];
	}
}

GameplayAbility_DeafaultAttack::~GameplayAbility_DeafaultAttack()
{
}

void GameplayAbility_DeafaultAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (IsActivating())
	{
		// NextCombo Event가 들어왔을 시
		if (EventName == "NextCombo")
		{
			if (m_ComboEnable)
			{
				int32 ComboCount = std::min((int32)(Combos.size() - 1), ++m_ComboCount);

				// 다음 콤보 애니메이션 실행
				AbilityUtils::PlayAnimation(this, Combos[ComboCount], false, false, true);

				// 초기화 해서 다시 재입력시 활성화
				m_ComboEnable = false;
			}
		}
		// Spawn Event가 들어왔을 시
		else if (EventName == "Spawn")
		{
			//NormalCollisionObject or FinalCollisionObject 중 현재 Combo에 맞게 소환
		}
	}
}

void GameplayAbility_DeafaultAttack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	AbilityUtils::PlayAnimation(this, Combos[0], false, false, true);
}

void GameplayAbility_DeafaultAttack::OnEndAbility()
{
	m_ComboCount = 0;
	m_ComboEnable = false;
}

void GameplayAbility_DeafaultAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	// 재입력 시 콤보가 발동되도록 설정
	m_ComboEnable = true;
}
