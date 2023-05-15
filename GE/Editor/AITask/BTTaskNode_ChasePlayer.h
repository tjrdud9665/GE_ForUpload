#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_ChasePlayer : public BTTaskNode
{
	CLASS(BTTaskNode_ChasePlayer, BTTaskNode)

protected:
	BTTaskNode_ChasePlayer();
	BTTaskNode_ChasePlayer(const BTTaskNode_ChasePlayer& Other);
	virtual ~BTTaskNode_ChasePlayer();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;
};

