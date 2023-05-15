#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_MonsterDefaultAttack : public BTTaskNode
{
	CLASS(BTTaskNode_MonsterDefaultAttack, BTTaskNode)

protected:
	BTTaskNode_MonsterDefaultAttack();
	BTTaskNode_MonsterDefaultAttack(const BTTaskNode_MonsterDefaultAttack& Other);
	virtual ~BTTaskNode_MonsterDefaultAttack();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, DefaultAttackRange, DetailsEdit)
	SBlackboardKeySelector DefaultAttackRange;
};

