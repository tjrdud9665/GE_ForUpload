#include "PCH.h"
#include "BTTaskNode_Patrol.h"
#include "GameFramework/GameObject/Character.h"
#include "GameFramework/Components/CharacterMovementComponent.h"

BTTaskNode_Patrol::BTTaskNode_Patrol()
{
	Location.AllowedTypes.push_back(EBlackBoardValType::Vector);
	PatrolRange.AllowedTypes.push_back(EBlackBoardValType::Float);
}

BTTaskNode_Patrol::BTTaskNode_Patrol(const BTTaskNode_Patrol& Other)
	:Super(Other)
	, Location(Other.Location)
	, PatrolRange(Other.PatrolRange)
{

}

BTTaskNode_Patrol::~BTTaskNode_Patrol()
{

}

ENodeStatus BTTaskNode_Patrol::UpdateNode(float DeltaSeconds)
{
	SVector3 Point = SVector3(0.0f);
	Location.GetBlackBoardValueAsVector(m_Tree, OUT Point);

	float Range = 0.0f;
	PatrolRange.GetBlackBoardValueAsFloat(m_Tree, OUT Range);

	if (Point != SVector3::ZeroVector && Range != 0.0f)
	{


		if (m_TargetPoint == SVector3::ZeroVector)
		{
			float x = SMathUtils::Random(-1, 1);
			float z = SMathUtils::Random(-1, 1);

			// ���� TargetPoint�� y���� Location���� ������ ��ġ�� y���� �Ȱ��� ������ �Ʒ��������� RayTest�� ���� ��, �� ���� �����̸� Picking�� �ȵ�����?
			m_TargetPoint = Point + Range * SVector3(x, 0.0f, z);
		}


		float dist = SVector3::Distance(m_Tree->GetControllerCharacter()->GetWorldLocationAsBottom(), m_TargetPoint);

		if (dist < 50.f)
		{
			m_Status = ENodeStatus::Success;
			m_TargetPoint = SVector3::ZeroVector;
		}

		else
		{
			m_Tree->GetControllerCharacter()->MoveToLocation(m_TargetPoint);
			m_Status = ENodeStatus::Running;
		}
	}
	else
		m_Status = ENodeStatus::Failure;

	return m_Status;
}