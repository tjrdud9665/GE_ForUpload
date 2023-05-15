#include "PCH.h"
#include "BTTaskNode_Wait5Seconds.h"

BTTaskNode_Wait5Seconds::BTTaskNode_Wait5Seconds()
	:m_WaitACCTime(0.0f)
{
}

BTTaskNode_Wait5Seconds::BTTaskNode_Wait5Seconds(const BTTaskNode_Wait5Seconds& Other)
	: Super(Other)
{
}

BTTaskNode_Wait5Seconds::~BTTaskNode_Wait5Seconds()
{
}

ENodeStatus BTTaskNode_Wait5Seconds::UpdateNode(float DeltaSeconds)
{
	m_WaitACCTime += DeltaSeconds;

	if (m_WaitACCTime >= 5.f)
	{
		m_Status = ENodeStatus::Success;
		m_WaitACCTime = 0.0f;
	}
	else
		m_Status = ENodeStatus::Running;

	return m_Status;
}
