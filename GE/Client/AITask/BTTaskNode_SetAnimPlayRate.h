#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_SetAnimPlayRate : public BTTaskNode
{
	CLASS(BTTaskNode_SetAnimPlayRate, BTTaskNode)

protected:
	BTTaskNode_SetAnimPlayRate();
	BTTaskNode_SetAnimPlayRate(const BTTaskNode_SetAnimPlayRate& Other);
	virtual ~BTTaskNode_SetAnimPlayRate();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, AnimPlayRate, DetailsEdit)
		SBlackboardKeySelector AnimPlayRate;
};

