#include "PCH.h"
#include "BTTaskNode_MoveTo.h"
#include "GameFramework/GameObject/Character.h"
#include "GameFramework/Components/CharacterMovementComponent.h"

BTTaskNode_MoveTo::BTTaskNode_MoveTo()
{
	Destination.AllowedTypes.push_back(EBlackBoardValType::Vector);
}

BTTaskNode_MoveTo::BTTaskNode_MoveTo(const BTTaskNode_MoveTo& Other)
	:Super(Other)
	,Destination(Other.Destination)
{

}

BTTaskNode_MoveTo::~BTTaskNode_MoveTo()
{

}

ENodeStatus BTTaskNode_MoveTo::UpdateNode(float DeltaSeconds)
{
	SVector3 dest = SVector3(0.0f);
	Destination.GetBlackBoardValueAsVector(m_Tree, OUT dest);

	if(dest != SVector3::ZeroVector)
	{
		SVector3 dist = m_Tree->GetControllerCharacter()->GetWorldLocationAsBottom() - dest;
		if (dist.Size() < 10.f)	// 68 : ÇöÀç Ä¸½¶ÀÇ Áö¸§
			m_Status = ENodeStatus::Success;
		else
		{
			m_Tree->GetControllerCharacter()->MoveToLocation(dest);
			m_Status = ENodeStatus::Running;
		}
	}
	return m_Status;
}
