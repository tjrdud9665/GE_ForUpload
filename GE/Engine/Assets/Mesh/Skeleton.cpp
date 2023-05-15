#include "EnginePCH.h"
#include "Skeleton.h"
#include "Path/PathManager.h"

Skeleton::Skeleton()
{

}

Skeleton::Skeleton(const Skeleton& Other)
	:Super(Other)
	, ModelName(Other.ModelName)
{

}

Skeleton::~Skeleton()
{
	m_BoneInfos.clear();
}

bool Skeleton::PostLoad()
{
	bool bResult = Super::PostLoad();

	size_t offsetMatrixSize = m_BoneInfos.size();
	m_OffsetMatrics.resize(offsetMatrixSize);
	for (size_t i = 0; i < offsetMatrixSize; i++)
	{
		m_OffsetMatrics[i] = m_BoneInfos[i].matOffset;
		//m_OffsetMatrics[i].Transpose();
	}


	return bResult;
}

bool Skeleton::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	//Save
	if (ar.IsSaving())
	{
		std::string RelPathWithName;
		GetRelativePathWithName(OUT RelPathWithName);
		fs::path bonePath = RelPathWithName;
		bonePath.replace_filename(GetName() + "_data.bin");
		std::string sPath = bonePath.string();
		ar.Serialize("bone_file_path", sPath);
		//Expression MeshContainer			;				
		const SPathInfo* Pathinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		fs::path fullPath = Pathinfo->aPath;
		fullPath /= bonePath;
		SaveMeshBoneFile(fullPath.string().c_str(),m_BoneInfos);
	}
	else
	{
		std::string RelPathWithName;
		GetRelativePathWithName(OUT RelPathWithName);
		fs::path bonePath = RelPathWithName;
		bonePath.replace_filename(GetName() + "_data.bin");
		std::string sPath = bonePath.string();
		ar.Serialize("bone_file_path", sPath);
		const SPathInfo* Pathinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		fs::path fullPath = Pathinfo->aPath;
		fullPath /= bonePath;

		LoadMeshBoneFile(fullPath.string().c_str(), OUT m_BoneInfos);
	}

	return bResult;
}

bool Skeleton::SaveMeshBoneFile(const char* FullPath, const std::vector<SBoneInfo>& InBoneInfos)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;


	size_t boneSize = InBoneInfos.size();
	::fwrite(&boneSize, sizeof(size_t), 1, File);
	for (size_t i = 0; i < boneSize; i++)
	{
		::fwrite(&InBoneInfos[i].matOffset, sizeof(Matrix), 1, File);
		::fwrite(&InBoneInfos[i].matBone, sizeof(Matrix), 1, File);
		::fwrite(&InBoneInfos[i].ParentIdx, sizeof(int32), 1, File);


		size_t	NameCount = InBoneInfos[i].Name.size();
		::fwrite(&NameCount, sizeof(size_t), 1, File);
		::fwrite(InBoneInfos[i].Name.c_str(), 1, NameCount, File);
		
		

	}

	fclose(File);

	return true;
}

bool Skeleton::LoadMeshBoneFile(const char* FullPath, std::vector<SBoneInfo>& OutBoneInfos)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	size_t boneSize = 0;
	::fread(&boneSize, sizeof(size_t), 1, File);
	OutBoneInfos.resize(boneSize);
	for (size_t i = 0; i < boneSize; i++)
	{
		::fread(&OutBoneInfos[i].matOffset, sizeof(Matrix), 1, File);
		::fread(&OutBoneInfos[i].matBone, sizeof(Matrix), 1, File);
		::fread(&OutBoneInfos[i].ParentIdx, sizeof(int32), 1, File);


		size_t	iNameCount = 0;
		char	BoneName[256] = {};
		::fread(&iNameCount, sizeof(size_t), 1, File);
		::fread(BoneName, 1, iNameCount, File);
		OutBoneInfos[i].Name = BoneName;

	}


	fclose(File);

	return true;
}

int32 Skeleton::GetRootBoneIndex() const
{
	size_t boneCount = m_BoneInfos.size();
	for(int32 i = 0 ;i<boneCount;i++)
	{
		if (m_BoneInfos[i].ParentIdx == INDEX_NONE)
			return i;
	}

	return 0;
	
}

size_t Skeleton::GetChilds(int32 boneIdx,std::vector<int32>& OutChildIdx)
{
	size_t boneCount = m_BoneInfos.size();
	for (int32 i = 0; i < boneCount; i++)
	{
		if (m_BoneInfos[i].ParentIdx == boneIdx)
			OutChildIdx.push_back(i);
	}
	return OutChildIdx.size();
}

int32 Skeleton::GetBoneCount() const
{
	return (int32)m_BoneInfos.size();
}

int32 Skeleton::GetParentIndex(const std::string& InName) const
{
	int32 idx = GetBoneIndex(InName);
	if(idx != INDEX_NONE)
	{
		return m_BoneInfos[idx].ParentIdx;
	}

	return INDEX_NONE;
}

Matrix Skeleton::GetBoneMatrix(int32 Index) const
{
	if (Index >= m_BoneInfos.size())
		return Matrix::StaticIdentity();

	return m_BoneInfos[Index].matOffset;
}

const std::string& Skeleton::GetBoneName(int32 Index) const
{
	return m_BoneInfos[Index].Name;
}



const Matrix& Skeleton::GetOffsetMatrixFromIndex(int32 Index) const
{
	return m_BoneInfos[Index].matOffset;
}

int32 Skeleton::GetBoneIndex(const std::string& InBoneName) const
{//TODO  : std::unordered_map<std::string, int32>	BoneMapping;
	
	for(size_t i =0;i<m_BoneInfos.size();i++)
	{
		if (m_BoneInfos[i].Name == InBoneName)
			return (int32)(i);
	}

	return INDEX_NONE;
}

void Skeleton::SetModelMeshName(const std::string& InName)
{
	ModelName = InName;
}


