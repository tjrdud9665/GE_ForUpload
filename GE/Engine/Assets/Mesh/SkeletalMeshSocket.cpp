#include "EnginePCH.h"
#include "SkeletalMeshSocket.h"
#include "GameFramework/Components/SkeletalMeshComponent.h"

SkeletalMeshSocket::SkeletalMeshSocket()
	: m_RelativeLocation(SVector3(0.0f))
	, m_RelativeRotation(SVector3(0.0f))
	, m_RelativeScale(SVector3(1.0f))
{

}

SkeletalMeshSocket::SkeletalMeshSocket(const SkeletalMeshSocket& Other)
	:Super(Other)
	, m_SocketName(Other.m_SocketName)
	, m_BoneName(Other.m_BoneName)
	, m_RelativeLocation(Other.m_RelativeLocation)
	, m_RelativeRotation(Other.m_RelativeRotation)
	, m_RelativeScale(Other.m_RelativeScale)
{

}

SkeletalMeshSocket::~SkeletalMeshSocket()
{

}

STransform SkeletalMeshSocket::GetSocketLocalTransform() const
{
	//m_RelativeRotation to Quat
	

	return STransform(SMathUtils::EulerToQuat(m_RelativeRotation), m_RelativeLocation, m_RelativeScale);
}

//SVector3 SkeletalMeshSocket::GetSocketLocation(const class SkeletalMeshComponent* SkelComp) const
//{
//	Matrix SocketMatrix;
//	if (GetSocketMatrix(SocketMatrix, SkelComp))
//	{
//		return SocketMatrix.GetTranslation();
//	}
//
//	// Fall back to MeshComp origin, so it's visible in case of failure.
//	return SkelComp->GetWorldLocation();
//}

//void SkeletalMeshSocket::InitializeSocketFromLocation(const class SkeletalMeshComponent* SkelComp, SVector3 WorldLocation, SVector3 WorldNormal)
//{
//	//m_BoneName = SkelComp->FindClosestBone(WorldLocation);
//	//if (m_BoneName != "")
//	//{
//	//	SkelComp->TransformToBoneSpace(m_BoneName, WorldLocation, WorldNormal, m_RelativeLocation, m_RelativeRotation);
//	//}
//}
