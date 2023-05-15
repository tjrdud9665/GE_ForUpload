#pragma once

#include "assimp/color4.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector2.h"
#include "assimp/quaternion.h"
#include "assimp/scene.h"
#include "assimp/ProgressHandler.hpp"
#include "assimp/version.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "Assets/Mesh/MeshAsset.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


#ifdef _DEBUG
	#pragma comment(lib, "assimp-vc142-mtd.lib")
#else
	#pragma comment(lib, "assimp-vc142-mt.lib")
#endif




//Model : public AssetData..
//class MeshAsset : public AssetData
//{
//public:
//	using MeshToMaterialLookup = std::unordered_map<FMeshID, TSharedPtr<class Material>>;
//	//vertex 만 같고 index 가 다른것은 다른 메쉬로 인식한다	
//	std::vector<std::vector<Mesh>>		MeshConatiner;	
//	MeshToMaterialLookup				MaterialLookupPerMesh;	
//};

struct LoadedData
{
	std::vector<std::vector<Mesh>>		MeshConatiner;	
	//std::unordered_map<FMeshID, TSharedPtr<class Material>>				MaterialLookupPerMesh;
};

struct SVertexBoneData
{
	SVector4	Weights;
	SVector4	Index;
	uint32		VertexID;
	std::vector<float>		vecWeigths;
	std::vector<uint32>		vecIndex;
};

struct SAssimpNode
{
	std::string		Name;	
	STransform		ParentTransform;


	SVector3		relPos;
	SVector4		relRotQuat;
	SVector3		relScale3D;

	bool			Root = false;

	std::vector<SVertex>	Vertices;
	std::vector<uint32>		Indices;

	struct aiMaterial*		MeshMaterial = nullptr;
	struct aiBone** MeshBone = nullptr;
	size_t		BoneCount = 0;
	SAssimpNode* Parent = nullptr;

	std::vector<SVertexBoneData> Bones;

	Matrix			testGlobalTrans;
	Matrix			testMatrix;
	Matrix			testMatrixInv;
	
};




class AssimpLoader
{
	friend class AnimationSequence;

public:
	AssimpLoader();
	~AssimpLoader();

	bool Load(class MeshAsset*& model, const char*FullPath);

	void Converting(class MeshAsset*& model, const aiScene* pAiScene);

private:
	// Parsing
	//class MeshAsset* ProcessNode(const aiNode* pNode, const aiScene* pAiScene, const char* FullPath, Renderer* pRenderer);

	void ParseNode(const aiNode* assimp_node, SAssimpNode* parent_node, SAssimpNode* new_Node, const aiScene* pAiScene);

	void ParseNodeMeshes(const aiNode* assimp_node, SAssimpNode* new_entity, const aiScene* pAiScene);
	void ParseAnimations(const aiScene* pAiScene,class Skeleton* ConvertedSkeleton);


	// Loading
	void LoadMesh(aiMesh* assimp_mesh, SAssimpNode* entity_parent, const aiScene* pAiScene);
	void LoadBones(const aiMesh* assimp_mesh, SAssimpNode* entity_parent, const aiScene* pAiScene);

private:
	void BoneTransform(float TimeInSeconds, std::vector<Matrix>& OutMatrix);

	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix& ParentTransform);

	


	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	

private:
	std::vector<SAssimpNode*>	m_EntityNodes;

	std::vector<SBoneInfo>		m_BoneInfo;
	std::unordered_map<std::string, uint32>		m_BoneMapping;
	uint32	m_NumBones = 0;
	fs::path				m_FullPath;

	const struct aiScene* m_pScene;
	
	Matrix			m_GlobalInverseTransform;
};

