#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_Wait5Seconds : public BTTaskNode
{
	CLASS(BTTaskNode_Wait5Seconds, BTTaskNode)

protected:
	BTTaskNode_Wait5Seconds();
	BTTaskNode_Wait5Seconds(const BTTaskNode_Wait5Seconds& Other);
	virtual ~BTTaskNode_Wait5Seconds();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	float m_WaitACCTime;
};

