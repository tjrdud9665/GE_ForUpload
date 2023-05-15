#include "EnginePCH.h"
#include "SkeletalMeshAsset.h"
#include "../../Renderer/Renderer.h"
#include "SkeletalMeshSocket.h"

SkeletalMeshAsset::SkeletalMeshAsset()
	:m_InstancingBoneMatrixArray(INDEX_NONE)
{

}

SkeletalMeshAsset::SkeletalMeshAsset(const SkeletalMeshAsset& Other)
	:Super(Other)
	, m_InstancingBoneMatrixArray(Other.m_InstancingBoneMatrixArray)
	,m_Skeleton(Other.m_Skeleton)
{
	//웬만하면 복제 하지말것.
}

SkeletalMeshAsset::~SkeletalMeshAsset()
{	
}

bool SkeletalMeshAsset::PostLoad()
{
	bool bResult = Super::PostLoad();

	return true;
}

bool SkeletalMeshAsset::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	ar.Serialize("m_Sockets", m_Sockets);
	size_t socketSize = m_Sockets.size();
	for(size_t i =0;i< socketSize;i++)
	{		//SetOwner
		m_Sockets[i]->SetOwner(this);
	}

	return true;
}

Skeleton* SkeletalMeshAsset::GetSkeleton() const
{
	return m_Skeleton;
}

int32 SkeletalMeshAsset::GetBoneCount() const
{
	if(m_Skeleton)
		return m_Skeleton->GetBoneCount();

	return 0;
}

FBufferID SkeletalMeshAsset::GetInstancingBoneMatrixArray() const
{
	return m_InstancingBoneMatrixArray;
}

FBufferID SkeletalMeshAsset::CreateInstancingBoneMatrix(class Renderer* pRenderer)
{
	int32 BoneCount = 100;
	if(m_Skeleton)
	{
		BoneCount = m_Skeleton->GetBoneCount();
		
	}
	
	m_InstancingBoneMatrixArray = pRenderer->CreateRWStructured("InstancingBoneMatrixArray", EShaders::COMPUTE_ANIMATION, nullptr, (int32)(BoneCount*(m_RenderCount+100)));

	return m_InstancingBoneMatrixArray;
}

size_t SkeletalMeshAsset::GetSkeletalMeshSocketFromBoneName(const std::string& BoneName, std::vector<class SkeletalMeshSocket*>& OutSockets) const
{
	size_t socketSize = m_Sockets.size();

	for (size_t i = 0; i < socketSize; i++)
	{
		if (m_Sockets[i]->m_BoneName == BoneName)
			OutSockets.push_back(m_Sockets[i]);
	}

	return OutSockets.size();
}

class SkeletalMeshSocket* SkeletalMeshAsset::GetSkeletalMeshSocket(const std::string& SocketName) const
{
	size_t socketSize = m_Sockets.size();

	for (size_t i = 0; i < socketSize; i++)
	{
		if (m_Sockets[i]->m_SocketName == SocketName)
			return m_Sockets[i];
	}
	return nullptr;
}

bool SkeletalMeshAsset::AddSkeletalMeshSocket(const std::string& InBoneName, const std::string& InSocketName)
{
	SkeletalMeshSocket* meshSocket = GetSkeletalMeshSocket(InSocketName);
	if(meshSocket)
	{
		return false;
	}
	if (InSocketName.empty())
		return false;

	meshSocket = Globals::CreateX<SkeletalMeshSocket>(this, "MeshSocket");
	meshSocket->m_SocketName = InSocketName;
	meshSocket->m_BoneName = InBoneName;

	m_Sockets.push_back(meshSocket);

	return true;
}

void SkeletalMeshAsset::RemoveSocket(class SkeletalMeshSocket* InSocket)
{
	std::vector<TSharedPtr<class SkeletalMeshSocket>>::iterator iter = m_Sockets.begin();
	std::vector<TSharedPtr<class SkeletalMeshSocket>>::iterator iter_end = m_Sockets.end();

	for(;iter != iter_end;iter++)
	{
		if ((*iter) == InSocket)
		{
			m_Sockets.erase((iter));
			return;
		}
	}

}

class SkeletalMeshSocket* SkeletalMeshAsset::FindSocketInfo(const std::string& InSocketName, STransform& OutTransform, int32& OutBoneIndex, int32& OutIndex) const
{
	OutIndex = INDEX_NONE;
	OutTransform = STransform::Identity;
	OutBoneIndex = INDEX_NONE;

	if (InSocketName == "")
	{
		return nullptr;
	}

	for (int32 i = 0; i < m_Sockets.size(); i++)
	{
		SkeletalMeshSocket* Socket = m_Sockets[i];
		if (Socket && Socket->m_SocketName == InSocketName)
		{
			OutIndex = i;
			OutTransform = Socket->GetSocketLocalTransform();
			OutBoneIndex = m_Skeleton->GetBoneIndex(Socket->m_BoneName);
			return Socket;
		}
	}


	return nullptr;
}
