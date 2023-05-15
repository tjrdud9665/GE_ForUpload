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
	// ���� Scene�� Player�� ã�ƿ´�.
	Character* Player = m_Tree->GetControllerCharacter()->GetOwningScene()->GetSceneInfo()->GetPlayerCharacter();

	if (Player)
	{
		// �÷��̾��� ��ġ
		SVector3 PlayerLocation = Player->GetWorldLocationAsBottom();

		// �÷��̾�� ���� ĳ���Ϳ��� �Ÿ�
		SVector3 dist = m_Tree->GetControllerCharacter()->GetWorldLocationAsBottom() - PlayerLocation;

		m_Tree->GetControllerCharacter()->MoveToLocation(PlayerLocation);
		m_Status = ENodeStatus::Success;
	}
	
	// ���� Scene�� Player�� ���ٸ� ����
	else
		m_Status = ENodeStatus::Failure;

	return m_Status;
}
