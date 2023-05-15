#include "PCH.h"
#include "BTTaskNode_SetAnimPlayRate.h"
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>

BTTaskNode_SetAnimPlayRate::BTTaskNode_SetAnimPlayRate()
{
	AnimPlayRate.AllowedTypes.push_back(EBlackBoardValType::Float);
}

BTTaskNode_SetAnimPlayRate::BTTaskNode_SetAnimPlayRate(const BTTaskNode_SetAnimPlayRate& Other)
	: Super(Other)
	, AnimPlayRate(Other.AnimPlayRate)
{
}

BTTaskNode_SetAnimPlayRate::~BTTaskNode_SetAnimPlayRate()
{
}

ENodeStatus BTTaskNode_SetAnimPlayRate::UpdateNode(float DeltaSeconds)
{
	float rate = 0.0f;
	AnimPlayRate.GetBlackBoardValueAsFloat(m_Tree, rate);

	if (rate != 0.0f)
	{
		m_Tree->GetControllerCharacter()->GetSkeletalMeshComponent()->SetPlayRate(rate);
		m_Status = ENodeStatus::Success;
	}
	else
		m_Status = ENodeStatus::Failure;

	return m_Status;
}
