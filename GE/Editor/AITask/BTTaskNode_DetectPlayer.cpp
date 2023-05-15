#include "PCH.h"
#include "BTTaskNode_DetectPlayer.h"
#include "GameFramework/GameObject/Character.h"
#include <Assets/Scene/Scene.h>

BTTaskNode_DetectPlayer::BTTaskNode_DetectPlayer()
{
	DetectRange.AllowedTypes.push_back(EBlackBoardValType::Float);
}

BTTaskNode_DetectPlayer::BTTaskNode_DetectPlayer(const BTTaskNode_DetectPlayer& Other)
	:Super(Other)
	, DetectRange(Other.DetectRange)
{
}

BTTaskNode_DetectPlayer::~BTTaskNode_DetectPlayer()
{
}

ENodeStatus BTTaskNode_DetectPlayer::UpdateNode(float DeltaSeconds)
{
	float range = 0.f;
	DetectRange.GetBlackBoardValueAsFloat(m_Tree, OUT range);

	if (range != 0.f)
	{
		Character* Mon = m_Tree->GetControllerCharacter();
		Character* Player = Mon->GetOwningScene()->GetSceneInfo()->GetPlayerCharacter();
		float dist = SVector3::Distance(Mon->GetWorldLocationAsBottom(), Player->GetWorldLocationAsBottom());

		//m_Status = ENodeStatus::Success;

		if (dist <= range)
		{
			// Detect되었을 때 
			//Mon->AddTag("Detect", true);
			m_Status = ENodeStatus::Success;
			//m_Tree->GetBlackBoard()->SetBool("Detect", true);
		}
		else
		{
			//Mon->AddTag("Detect", false);
			m_Status = ENodeStatus::Failure;
			//m_Tree->GetBlackBoard()->SetBool("Detect", false);
		}

	}
	else
	{
		// DetectRange가 없을 경우는 탐지 실패
		m_Status = ENodeStatus::Failure;
	}

	return m_Status;
}