#pragma once
#include "../AssetData.h"

//
//struct SBoneInfo
//{
//	std::string			Name;
//	Matrix				matOffset;
//	Matrix				matBone;	
//
//};

class Skeleton :public AssetData
{
	CLASS(Skeleton ,AssetData)
	friend class AssimpLoader;
	friend class FBXLoader;


protected:
	Skeleton();
	Skeleton(const Skeleton& Other);
	virtual ~Skeleton();
	
public:
	//모든 Bone 데이터가 일치한다면
	virtual bool PostLoad() override;

	virtual bool Serialize(Archive& ar) override;

	static bool SaveMeshBoneFile(const char* FullPath, const std::vector<SBoneInfo>& InBoneInfos);
	static bool LoadMeshBoneFile(const char* FullPath, std::vector<SBoneInfo>& OutBoneInfos);


public:
	int32 GetRootBoneIndex() const;

	size_t GetChilds(int32 boneIdx,std::vector<int32>& OutChildIdx);

	int32 GetBoneCount() const;

	int32 GetParentIndex(const std::string& InName) const;	
		

	Matrix GetBoneMatrix(int32 Index) const;

	const std::string& GetBoneName(int32 Index) const;	
	const Matrix& GetOffsetMatrixFromIndex(int32 Index) const;
	int32 GetBoneIndex(const std::string& InBoneName) const;

	void SetModelMeshName(const std::string& InName);

	const std::string& GetModelMeshName() const
	{
		return ModelName;
	}

	const std::vector<Matrix>& GetOffsetMatrics()
	{
		if(m_BoneInfos.size() != m_OffsetMatrics.size())
		{
			size_t boneSize = m_BoneInfos.size();
			m_OffsetMatrics.resize(boneSize);

			for(size_t i = 0; i<boneSize;i++)
			{
				m_OffsetMatrics[i] = m_BoneInfos[i].matOffset;
			}
		}

		return m_OffsetMatrics;
	}

private:
	std::vector<SBoneInfo>				m_BoneInfos;
	std::vector<Matrix>					m_OffsetMatrics;

	std::vector<class SkeletalMeshSocket*>	m_BoneSocket;
		
	PROP(std::string, ModelName,DetailsView,"Mesh")
	std::string		ModelName;



};

