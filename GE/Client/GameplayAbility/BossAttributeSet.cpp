#include "PCH.h"
#include "BossAttributeSet.h"
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/GameObject/AIController.h>
#include <GameFramework/Components/CharacterMovementComponent.h>

BossAttributeSet::BossAttributeSet()
	: m_isPhase1(false)
	, m_isPhase2(false)
	, m_isPhase3(false)
{
}

BossAttributeSet::BossAttributeSet(const BossAttributeSet& Other)
	: Super(Other)
	, m_isPhase1(Other.m_isPhase1)
	, m_isPhase2(Other.m_isPhase2)
	, m_isPhase3(Other.m_isPhase3)
{
}

BossAttributeSet::~BossAttributeSet()
{
}

void BossAttributeSet::PostGameplayEffectExecute(GameplayEffect* InDef, AbilityComponent* Src, AbilityComponent* Target)
{
	Super::PostGameplayEffectExecute(InDef, Src, Target);

	if (Health.GetCurrentValue() != 0.0f)
	{
		float HealthRate = Health.GetCurrentValue() / MaxHealth.GetCurrentValue();

		Character* ownerChar = GetOwner<Character>();

		if (HealthRate <= 0.3f)
		{
			if (!m_isPhase3)
			{
				AIController* aiCon = ownerChar->GetControllerAs<AIController>();

				if (aiCon)
				{
					aiCon->SetBlackboardValueAsBool("Phase3", true);
					aiCon->RemoveBlackboardValue("Phase1", EBlackBoardValType::Bool);
					aiCon->RemoveBlackboardValue("Phase2", EBlackBoardValType::Bool);
					aiCon->SetBlackboardValueAsFloat("AnimRate", 1.f);
				}
				float speed = MoveSpeed.GetBaseValue() * 2.f;
				MoveSpeed.SetBaseValue(speed);
				MoveSpeed.SetCurrentValue(speed);
				GetOwner<Character>()->GetMovementComponent()->SetMaxSpeed(speed);

				m_isPhase3 = true;
			}
		}
		else if (HealthRate <= 0.7f)
		{
			if (!m_isPhase2)
			{
				AIController* aiCon = ownerChar->GetControllerAs<AIController>();

				if (aiCon)
				{
					aiCon->SetBlackboardValueAsBool("Phase2", true);
					aiCon->RemoveBlackboardValue("Phase1", EBlackBoardValType::Bool);
					aiCon->RemoveBlackboardValue("Phase3", EBlackBoardValType::Bool);
					aiCon->SetBlackboardValueAsFloat("AnimRate", 0.8f);
				}
				m_isPhase2 = true;
			}
		}
		else
		{
			if (!m_isPhase1)
			{
				AIController* aiCon = ownerChar->GetControllerAs<AIController>();

				if (aiCon)
				{
					aiCon->SetBlackboardValueAsBool("Phase1", true);
					aiCon->RemoveBlackboardValue("Phase2", EBlackBoardValType::Bool);
					aiCon->RemoveBlackboardValue("Phase3", EBlackBoardValType::Bool);
					aiCon->SetBlackboardValueAsFloat("AnimRate", 0.6f);
				}
				m_isPhase1 = true;
			}
		}
	}
}
