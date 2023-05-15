#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_MoveTo :public BTTaskNode
{
	CLASS(BTTaskNode_MoveTo, BTTaskNode)

protected:
	BTTaskNode_MoveTo();
	BTTaskNode_MoveTo(const BTTaskNode_MoveTo& Other);
	virtual ~BTTaskNode_MoveTo();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, Destination, DetailsEdit)
	SBlackboardKeySelector Destination;
};

