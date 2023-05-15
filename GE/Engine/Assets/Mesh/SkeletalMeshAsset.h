#pragma once
#include "MeshAsset.h"
#include "Skeleton.h"

class SkeletalMeshAsset :public MeshAsset
{
	CLASS(SkeletalMeshAsset,MeshAsset)
	friend class AssimpLoader;
	friend class FBXLoader;
	friend class SkeletalMeshComponent;

protected:
	SkeletalMeshAsset();
	SkeletalMeshAsset(const SkeletalMeshAsset& Other);
	virtual ~SkeletalMeshAsset();


	virtual bool PostLoad() override;

	virtual bool Serialize(Archive& ar) override;

public:
	Skeleton* GetSkeleton() const;

	int32		GetBoneCount() const;

	FBufferID		GetInstancingBoneMatrixArray() const;

	FBufferID		CreateInstancingBoneMatrix(class Renderer* pRenderer);

	size_t GetSkeletalMeshSocketFromBoneName(const std::string& BoneName,std::vector<class SkeletalMeshSocket*>& OutSockets) const;
		
	class SkeletalMeshSocket* GetSkeletalMeshSocket(const std::string& SocketName) const;


	bool AddSkeletalMeshSocket(const std::string& InBoneName, const std::string& InSocketName);

	void RemoveSocket(class SkeletalMeshSocket* InSocket);


public:
	class SkeletalMeshSocket* FindSocketInfo(const std::string& InSocketName, STransform& OutTransform, int32& OutBoneIndex, int32& OutIndex) const;

private:
	PROP(TSharedPtr<Skeleton> , m_Skeleton ,DetailsEdit)
	TSharedPtr<Skeleton>			m_Skeleton;

	FBufferID						m_InstancingBoneMatrixArray;

	
	//PROP(std::vector<TSharedPtr<class SkeletalMeshSocket>>, m_Sockets, DetailsEdit)
	std::vector<TSharedPtr<class SkeletalMeshSocket>>		m_Sockets;
};

