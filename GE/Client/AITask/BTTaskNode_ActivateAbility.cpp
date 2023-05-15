#include "PCH.h"
#include "BTTaskNode_ActivateAbility.h"
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <Assets/Scene/Scene.h>

BTTaskNode_ActivateAbility::BTTaskNode_ActivateAbility()
{
	Ability.AllowedTypes.push_back(EBlackBoardValType::Ability);
	AbilityPlayRate.AllowedTypes.push_back(EBlackBoardValType::Float);
	AbilityRange.AllowedTypes.push_back(EBlackBoardValType::Float);
}

BTTaskNode_ActivateAbility::BTTaskNode_ActivateAbility(const BTTaskNode_ActivateAbility& Other)
	:Super(Other)
	, Ability(Other.Ability)
	, AbilityPlayRate(Other.AbilityPlayRate)
	, AbilityRange(Other.AbilityRange)
{
}

BTTaskNode_ActivateAbility::~BTTaskNode_ActivateAbility()
{
}

ENodeStatus BTTaskNode_ActivateAbility::UpdateNode(float DeltaSeconds)
{
	TSubclassOf<GameplayAbility> ability;
	Ability.GetBlackBoardValueAsAbility(m_Tree, OUT ability);

	float abilityRange = 0.f;
	AbilityRange.GetBlackBoardValueAsFloat(m_Tree, OUT abilityRange);

	if ((!ability.CustomCDOName.empty()) && (abilityRange != 0.0f))
	{
		Character* Mon = m_Tree->GetControllerCharacter();
		Character* Player = Mon->GetOwningScene()->GetSceneInfo()->GetPlayerCharacter();
		float dist = SVector3::Distance(Mon->GetWorldLocationAsBottom(), Player->GetWorldLocationAsBottom());

		if (dist <= abilityRange)
		{

			if (Mon->GetAbilityComponent()->TryActivateAbilityByClass(ability))
			{
				float playRate = 0.f;
				AbilityPlayRate.GetBlackBoardValueAsFloat(m_Tree, OUT playRate);

				if (playRate != 0.0f)
					Mon->GetSkeletalMeshComponent()->SetPlayRate(playRate);

			}
			m_Status = ENodeStatus::Success;
		}
		else
			m_Status = ENodeStatus::Running;

	}
	else
	{
		m_Status = ENodeStatus::Failure;
	}

	return m_Status;
}
