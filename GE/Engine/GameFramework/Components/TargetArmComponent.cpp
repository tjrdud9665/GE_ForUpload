#include "EnginePCH.h"
#include "TargetArmComponent.h"
#include <RenderManager.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/BulletPhysicsHelper.h>
#include <Physics/PhysicsDebugDraw.h>

TargetArmComponent::TargetArmComponent()
	:m_TargetOffset(0.0f)
	, m_TargetSpeed(12.0f)
	, m_TargetDistanceAxis(EAxis::Z)
{

}

TargetArmComponent::TargetArmComponent(const TargetArmComponent& Other)
	:Super(Other)
	, m_TargetOffset(Other.m_TargetOffset)
	, m_TargetDistance(Other.m_TargetDistance)
	, m_TargetDistanceAxis(Other.m_TargetDistanceAxis)
	, m_TargetSpeed(Other.m_TargetSpeed)
{

}

TargetArmComponent::~TargetArmComponent()
{

}

void TargetArmComponent::BeginPlay()
{
	Super::BeginPlay();
}

void TargetArmComponent::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
	SceneComponent* parent = GetAttachParent();
	if (parent)
	{
		SVector3	ParentPos = parent->GetWorldLocation();
		SVector3 ConvertRot = GetWorldRotation().ConvertAngle();
		DirectX::XMVECTOR	Qut = DirectX::XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);
					
		SVector3 Pos = ParentPos - GetWorldAxis(m_TargetDistanceAxis.Get()) * m_TargetDistance;		

		SVector3 targetPos = Pos;
		Matrix	matRot;
		matRot.RotationQuaternion(Qut);
		SVector3 RotatedOffset = m_TargetOffset.TransformCoord(matRot);
		RotatedOffset.z = 0;
		SVector3 lerped = SVector3::VInterpTo(m_PrevPos, targetPos + RotatedOffset, DeltaSeconds, m_TargetSpeed);

		SetWorldLocation(lerped);

	}
	
	SetAbsoluteLocation(true);
	SetAbsoluteRotation(true);
	
}


void TargetArmComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	m_PrevPos = GetWorldLocation();	

	SceneComponent* parent = GetAttachParent();
	if (parent)
	{
		if (!IsPlaying() && g_Engine->IsEditMode())
		{
			//DrawLine TO Target
			g_Engine->Get<RenderManager>()->DrawLine(GetWorldLocation(), parent->GetWorldLocation(), SLinearColor::Red);
			
		}

	}

}
