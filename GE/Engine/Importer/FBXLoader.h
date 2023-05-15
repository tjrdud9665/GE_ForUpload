#pragma once


#include "fbxsdk.h"


#ifdef _DEBUG

#pragma comment(lib, "libfbxsdk-md_Debug.lib")

#else

#pragma comment(lib, "libfbxsdk-md.lib")

#endif // _DEBUG

struct FbxBone
{
	std::string     Name;
	int             Depth;
	int             ParentIndex;
	FbxAMatrix      matOffset;
	FbxAMatrix      matBone;
};




struct FbxKeyFrame
{
	FbxAMatrix	matTransform;
	double		Time;
};

struct FbxBoneKeyFrame
{
	int			BoneIndex;
	std::vector<FbxKeyFrame>	vecKeyFrame;
};

struct FbxAnimationClip
{
	std::string		Name;
	FbxTime			StartTime;
	FbxTime			EndTime;
	FbxLongLong		TimeLength;
	FbxTime::EMode	TimeMode;
	std::vector<FbxBoneKeyFrame>	vecBoneKeyFrame;
};


struct FbxWeight
{
	int		Index;
	double	Weight;
};

struct FbxMaterial
{
	SVector4	BaseColor;
	SVector4	AmbientColor;
	SVector4	SpecularColor;
	SVector4	EmissiveColor;
	float	SpecularPower;
	float	TransparencyFactor;
	float	Shininess;
	std::string	DiffuseTexture;
	std::string	BumpTexture;
	std::string	SpecularTexture;

	std::string MaterialName;
};

struct FbxMeshContainer
{
	std::vector<SVector3>	vecPos;
	std::vector<SVector3>	vecNormal;
	std::vector<SVector2>	vecUV;
	std::vector<SVector3>	vecTangent;
	std::vector<SVector3>	vecBinormal;
	std::vector<SVector4>	vecBlendWeight;
	std::vector<SVector4>	vecBlendIndex;
	std::vector<std::vector<uint32>>	vecIndex;
	std::unordered_map<int, std::vector<FbxWeight>>	mapWeights;
	bool	Bump;
	bool	Animation;
	std::string ModelName;

	SVector3 bbMin;
	SVector3 bbMax;


	FbxMeshContainer() :
		Bump(false),
		Animation(false)
	{}
};


class FBXLoader
{
	friend class Mesh;
	friend class StaticMesh;
	friend class Skeleton;
	friend class SkeletalMesh;
	friend class AnimationSequence;
	friend class AssetManager;

private:
	FBXLoader();
	~FBXLoader();


public:
	bool LoadFBX(const char* FullPath, MeshAsset*& OutMeshAsset);

	void Converting(MeshAsset*& OutMeshAsset);
	



	bool IsConverted(const char* FullPath);


private:
	void Triangulate(FbxNode* Node);

	void LoadMaterial(FbxSurfaceMaterial* Mtrl,const char* MtrlName);

	SVector4 GetMaterialColor(FbxSurfaceMaterial* Mtrl,const char* PropertyName, const char* PropertyFactorName);
	double GetMaterialFactor(FbxSurfaceMaterial* Mtrl,const char* PropertyName);
	std::string GetMaterialTexture(FbxSurfaceMaterial* Mtrl,const char* PropertyName);

	void LoadMesh(FbxNode* Node, bool Static);
	void LoadMesh(FbxMesh* Mesh, bool Static ,const char* InName);

	void LoadNormal(FbxMesh* Mesh, FbxMeshContainer* Container,int32 VtxID,int32 ControlIndex);

	void LoadUV(FbxMesh* Mesh, FbxMeshContainer* pContainer, int32 UVID, int32 ControlIndex, int32 VtxID, const FbxArray<FbxVector2>& uvs);
	void LoadTangent(FbxMesh* Mesh,FbxMeshContainer* Container, int32 VtxID, int32 ControlIndex);
	void LoadBinormal(FbxMesh* Mesh, FbxMeshContainer* Container, int32 VtxID, int32 ControlIndex);


private:
	void LoadAnimationClip();
	void LoadBone(FbxNode* Node);
	void LoadBoneRecursive(FbxNode* Node, int32 Depth, int32 Index, int32 Parent);
	void LoadAnimation(FbxMesh* Mesh, FbxMeshContainer* Container);
	FbxAMatrix GetTransform(FbxNode* Node);
	int32 FindBoneFromName(const std::string& Name);
	void LoadWeightAndIndex(FbxCluster* Cluster,
							int BoneIndex, FbxMeshContainer* Container);
	void LoadOffsetMatrix(FbxCluster* Cluster,
						  const FbxAMatrix& matTransform, int BoneIndex,
						  FbxMeshContainer* Container);
	void LoadTimeTransform(FbxNode* Node,
						   FbxCluster* Cluster, const FbxAMatrix& matTransform,
						   int BoneIndex);
	void ChangeWeightAndIndices(FbxMeshContainer* Container);

public:
	const std::vector<FbxMeshContainer*>* GetContainer()	const
	{
		return &m_vecMeshContainer;
	}

	const std::vector<std::vector<FbxMaterial*>>* GetMaterials()	const
	{
		return &m_vecMaterial;
	}

	const std::vector<FbxBone*>* GetBones()	const
	{
		return &m_vecBone;
	}

	const std::vector<FbxAnimationClip*>* GetClips()	const
	{
		return &m_vecClip;
	}

private:
	FbxManager* m_Manager;
	FbxScene* m_Scene;


private:
	std::vector<FbxMeshContainer*>	m_vecMeshContainer;
	std::vector<std::vector<FbxMaterial*>>	m_vecMaterial;
	std::vector<FbxBone*>			m_vecBone;
	FbxArray<FbxString*>			m_NameArray;
	std::vector<FbxAnimationClip*>	m_vecClip;


	bool						m_ReqAnimOnly;
	


private:
	fs::path				m_FullPath;


};

