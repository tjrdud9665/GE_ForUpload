#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_ActivateAbility : public BTTaskNode
{
	CLASS(BTTaskNode_ActivateAbility, BTTaskNode)

protected:
	BTTaskNode_ActivateAbility();
	BTTaskNode_ActivateAbility(const BTTaskNode_ActivateAbility& Other);
	virtual ~BTTaskNode_ActivateAbility();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, Ability, DetailsEdit)
	SBlackboardKeySelector Ability;

	PROP(SBlackboardKeySelector, AbilityPlayRate, DetailsEdit)
		SBlackboardKeySelector AbilityPlayRate;

	PROP(SBlackboardKeySelector, AbilityRange, DetailsEdit)
	SBlackboardKeySelector AbilityRange;
};

