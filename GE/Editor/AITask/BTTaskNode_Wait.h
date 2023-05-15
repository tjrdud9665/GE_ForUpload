#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_Wait : public BTTaskNode
{
	CLASS(BTTaskNode_Wait, BTTaskNode)

protected:
	BTTaskNode_Wait();
	BTTaskNode_Wait(const BTTaskNode_Wait& Other);
	virtual ~BTTaskNode_Wait();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, WaitSeconds, DetailsEdit)
	SBlackboardKeySelector WaitSeconds;

	float m_WaitACCTime;
};

