#pragma once
#include "XObject/XObject.h"

/* copy From Unreal Engine's USkeletalMeshSocket */

/*
* Rotation Expression Euler Deg.
*/


class SkeletalMeshSocket : public XObject
{
	CLASS(SkeletalMeshSocket, XObject)
	friend class Skeleton;
	friend class SkeletalMeshAsset;

public:
	SkeletalMeshSocket();
	SkeletalMeshSocket(const SkeletalMeshSocket& Other);
	virtual ~SkeletalMeshSocket();
		
public:
	SVector3 GetSocketLocation(const class SkeletalMeshComponent* SkelComp) const;

	STransform GetSocketLocalTransform() const;

	const std::string& GetSocketName() const
	{
		return m_SocketName;
	}
	const std::string& GetBoneName() const
	{
		return m_BoneName;
	}

	void SetRelativeLocation(const SVector3& InLocation)
	{
		m_RelativeLocation = InLocation;
	}

	void SetRelativeRotation(const SVector3& InRotation)
	{
		m_RelativeRotation = InRotation;
	}

	void SetRelativeScale(const SVector3& InScale)
	{
		m_RelativeScale = InScale;
	}



	const SVector3& GetRelativeLocation() const
	{
		return m_RelativeLocation;
	}

	const SVector3& GetRelativeRotation() const
	{
		return m_RelativeRotation;
	}

	const SVector3& GetRelativeScale() const
	{
		return m_RelativeScale;
	}

private:
	PROP(std::string, m_SocketName,DetailsEdit,"SocketInfo")
	std::string m_SocketName;

	PROP(std::string, m_BoneName, DetailsView, "SocketInfo")
	std::string m_BoneName;

	//SocketIndex


	PROP(SVector3, m_RelativeLocation, DetailsEdit, "SocketInfo")
	SVector3 m_RelativeLocation;
	
	PROP(SVector3, m_RelativeRotation, DetailsEdit, "SocketInfo")
	SVector3 m_RelativeRotation;
	
	PROP(SVector3, m_RelativeScale, DetailsEdit, "SocketInfo")
	SVector3 m_RelativeScale;


};

