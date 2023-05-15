#include "PCH.h"
#include "BTTaskNode_MonsterDefaultAttack.h"
#include "../GameObject/Monster.h"
#include <GameFramework/Components/AbilityComponent.h>
#include <Assets/Scene/Scene.h>

BTTaskNode_MonsterDefaultAttack::BTTaskNode_MonsterDefaultAttack()
{
	DefaultAttackRange.AllowedTypes.push_back(EBlackBoardValType::Float);
}

BTTaskNode_MonsterDefaultAttack::BTTaskNode_MonsterDefaultAttack(const BTTaskNode_MonsterDefaultAttack& Other)
	:Super(Other)
	, DefaultAttackRange(Other.DefaultAttackRange)
{
}

BTTaskNode_MonsterDefaultAttack::~BTTaskNode_MonsterDefaultAttack()
{
}

ENodeStatus BTTaskNode_MonsterDefaultAttack::UpdateNode(float DeltaSeconds)
{
	Monster* Mon = dynamic_cast<Monster*>(m_Tree->GetControllerCharacter());

	float AttackRange = 0.f;
	DefaultAttackRange.GetBlackBoardValueAsFloat(m_Tree, OUT AttackRange);

	if (Mon && AttackRange != 0.f)
	{
		Character* Player = Mon->GetOwningScene()->GetSceneInfo()->GetPlayerCharacter();
		float dist = SVector3::Distance(Mon->GetWorldLocationAsBottom(), Player->GetWorldLocationAsBottom());

		if (dist <= AttackRange)
		{
			Mon->DefaultAttack();
			m_Status = ENodeStatus::Success;
		}
		else
			m_Status = ENodeStatus::Failure;

	}
	else
	{
		m_Status = ENodeStatus::Failure;
	}

	return m_Status;
}
