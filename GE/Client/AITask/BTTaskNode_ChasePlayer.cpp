#include "PCH.h"
#include "BTTaskNode_ChasePlayer.h"
#include "GameFramework/GameObject/Character.h"
#include "GameFramework/Components/CharacterMovementComponent.h"
#include <Assets/Scene/Scene.h>

BTTaskNode_ChasePlayer::BTTaskNode_ChasePlayer()
{
}

BTTaskNode_ChasePlayer::BTTaskNode_ChasePlayer(const BTTaskNode_ChasePlayer& Other)
	:Super(Other)
{
}

BTTaskNode_ChasePlayer::~BTTaskNode_ChasePlayer()
{
}

ENodeStatus BTTaskNode_ChasePlayer::UpdateNode(float DeltaSeconds)
{
	// 현재 Scene의 Player을 찾아온다.
	Character* Player = m_Tree->GetControllerCharacter()->GetOwningScene()->GetSceneInfo()->GetPlayerCharacter();

	if (Player)
	{
		// 플레이어의 위치
		SVector3 PlayerLocation = Player->GetWorldLocationAsBottom();

		// 플레이어와 현재 캐릭터와의 거리
		SVector3 dist = m_Tree->GetControllerCharacter()->GetWorldLocationAsBottom() - PlayerLocation;

		m_Tree->GetControllerCharacter()->MoveToLocation(PlayerLocation);
		m_Status = ENodeStatus::Success;
	}
	
	// 현재 Scene의 Player가 없다면 실패
	else
		m_Status = ENodeStatus::Failure;

	return m_Status;
}
