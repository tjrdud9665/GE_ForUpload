#include "PCH.h"
#include "BTTaskNode_Wait.h"

BTTaskNode_Wait::BTTaskNode_Wait()
	:m_WaitACCTime(0.0f)
{
	WaitSeconds.AllowedTypes.push_back(EBlackBoardValType::Float);
}

BTTaskNode_Wait::BTTaskNode_Wait(const BTTaskNode_Wait& Other)
	:Super(Other)
	, WaitSeconds(Other.WaitSeconds)
{
}

BTTaskNode_Wait::~BTTaskNode_Wait()
{
}

ENodeStatus BTTaskNode_Wait::UpdateNode(float DeltaSeconds)
{
	float WaitTime = 0.0f;
	WaitSeconds.GetBlackBoardValueAsFloat(m_Tree, WaitTime);

	if (WaitTime != 0.0f)
	{
		m_WaitACCTime += DeltaSeconds;

		if (m_WaitACCTime >= WaitTime)
		{
			m_Status = ENodeStatus::Success;
			m_WaitACCTime = 0.0f;
		}
		else
			m_Status = ENodeStatus::Running;

	}
	else
		m_Status = ENodeStatus::Failure;

	return m_Status;
}
