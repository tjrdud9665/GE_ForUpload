#pragma once
#include "../AssetData.h"
#include "Mesh.h"			//TO Render/Mesh로 변경



//Static// Skeletal Mesh
class MeshAsset :public AssetData
{
	CLASS(MeshAsset, AssetData)	
	friend class PhysicsSystem;
	friend class FBXLoader;
	using FMeshID = int32;
	using FMeshToMaterialLookup = std::unordered_map<FMeshID, TSharedPtr<class Material>>;
	//Slot 에 따른 머터리얼 
	

protected:
	MeshAsset();
	MeshAsset(const MeshAsset& Other);
	virtual ~MeshAsset();

	virtual bool PostLoad() override;


	virtual bool Serialize(Archive& ar) override;

private:
	/* Write VertexData to Binary File */
	static bool SaveMeshVtxFile(const char* FullPath, const std::vector<std::vector<SVertex>>& vecContainer, const std::vector<std::vector<uint32>>& vecIndexContainer, const std::vector<std::string>& Names, bool Skeletal ,const SMeshCollisionData& CollisionData);

	static 	bool LoadMeshVtxFile(const char* FullPath, std::vector<std::vector<SVertex>>& OutContainer,std::vector<std::vector<uint32>>& OutIndex,std::vector<std::string>& OutNames, bool Skeletal, SMeshCollisionData& OutCollisionData);
	
	
public:
	//For Rendering
	int32 GetMeshCount() const
	{
		return (int32)m_MeshContainer.size();
	}

	int32 GetRenderCount() const
	{
		return m_RenderCount;
	}
	
	void AddRenderCount(bool Add = true);


	const Mesh& GetMesh(int32 SlotIndex) const
	{
		if(SlotIndex >= m_MeshContainer.size())
		{
			SlotIndex = (int32)m_MeshContainer.size() -1;

		}
		return m_MeshContainer[SlotIndex];
	}

	int32 AddMesh(const Mesh& InMesh);
	
	void AddMaterial(int32 SlotIdx,class Material* InMaterial);


	class Material* GetMaterial(int32 SlotIndex) const
	{
		FMeshToMaterialLookup::const_iterator iter = m_MaterialLookupPerMesh.find(SlotIndex);
		
		if (iter != m_MaterialLookupPerMesh.end())
			return (*iter).second;

		return nullptr;		
	}
	size_t GetMaterials(std::vector<class Material*>& OutMaterials) const
	{
		FMeshToMaterialLookup::const_iterator iter = m_MaterialLookupPerMesh.begin();
		FMeshToMaterialLookup::const_iterator iter_end = m_MaterialLookupPerMesh.end();

		for (; iter != iter_end; iter++)
		{
			OutMaterials.push_back((*iter).second->Clone());
		}

		return OutMaterials.size();
	}

	size_t GetMaterials_NotCloned(std::vector<class Material*>& OutMaterials) const
	{
		FMeshToMaterialLookup::const_iterator iter = m_MaterialLookupPerMesh.begin();
		FMeshToMaterialLookup::const_iterator iter_end = m_MaterialLookupPerMesh.end();

		for (; iter != iter_end; iter++)
		{
			OutMaterials.push_back((*iter).second);
		}

		return OutMaterials.size();
	}

	size_t GetMaterials(std::vector<TSharedPtr<class Material>>& OutMaterials) const
	{
		OutMaterials.clear();
		FMeshToMaterialLookup::const_iterator iter = m_MaterialLookupPerMesh.begin();
		FMeshToMaterialLookup::const_iterator iter_end = m_MaterialLookupPerMesh.end();

		for(;iter != iter_end;iter++)
		{
			OutMaterials.push_back((*iter).second->Clone());
		}		

		return OutMaterials.size();
	}

	//Only Use In Editor !!!!!
	void SetMaterials(const std::vector<class Material*>& InMaterials)
	{
		FMeshToMaterialLookup::iterator iter = m_MaterialLookupPerMesh.begin();
		FMeshToMaterialLookup::iterator iter_end = m_MaterialLookupPerMesh.end();
		int32 i = 0;
		for (; iter != iter_end; iter++,i++)
		{
			iter->second = InMaterials[i];
		}
	}
	
	void ComputeBB(const std::vector<SVertex>& InVertices);


	void AddCollisionMeshData(const std::vector<SVector3>& vertices, const std::vector<uint32>& indices);

	const SMeshCollisionData& GetMeshCollisionData() const;

public:
	void GetAABB(SVector3& outBBMin, SVector3& outBBMax);

	

protected:
	std::vector<Mesh>					m_MeshContainer;	
	FMeshToMaterialLookup				m_MaterialLookupPerMesh;

	//Material Slot 을 Editor 에 노출할것인가?
	//Mesh Editor 에서 MaterialSlot 의 값을 변경 할 수 있도록 에디터 유도	
	int32					m_RenderCount;
	
	SVector3			m_BBMin;
	SVector3			m_BBMax;

private:
	SMeshCollisionData	m_CollisionData;
};

