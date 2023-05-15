#pragma once
#include <Assets/AI/BehaviorTree.h>

class BTTaskNode_Patrol : public BTTaskNode
{
	CLASS(BTTaskNode_Patrol, BTTaskNode)

protected:
	BTTaskNode_Patrol();
	BTTaskNode_Patrol(const BTTaskNode_Patrol& Other);
	virtual ~BTTaskNode_Patrol();

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override;

protected:
	PROP(SBlackboardKeySelector, Location, DetailsEdit)
	SBlackboardKeySelector Location;

	PROP(SBlackboardKeySelector, PatrolRange, DetailsEdit)
	SBlackboardKeySelector PatrolRange;

	SVector3	m_TargetPoint;
};

