#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_DetectPlayer : public BTTaskNode
{
	CLASS(BTTaskNode_DetectPlayer, BTTaskNode)

protected:
	BTTaskNode_DetectPlayer();
	BTTaskNode_DetectPlayer(const BTTaskNode_DetectPlayer& Other);
	virtual ~BTTaskNode_DetectPlayer();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, DetectRange, DetailsEdit)
	SBlackboardKeySelector DetectRange;
};

