#include "EnginePCH.h"
#include "MeshAsset.h"
#include <Renderer/Renderer.h>
#include <Renderer/D3DBuffer.h>
#include <Path/PathManager.h>
#include "SkeletalMeshAsset.h"
#include <Structs.h>
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"

MeshAsset::MeshAsset()
	:m_BBMin(SVector3(FLT_MAX))
	,m_BBMax(SVector3(FLT_MIN))
{

}

MeshAsset::MeshAsset(const MeshAsset& Other)
	:Super(Other)
{

}

MeshAsset::~MeshAsset()
{	
	SAFE_DELETE(m_CollisionData.CollisonVertexArray);
}

bool MeshAsset::PostLoad()
{
	JsonArchive ar(GetData());
	//Already Loaded.
	if (m_MeshContainer.size())
	{
		return true;
	}
	Serialize(ar);		//Load Vertex , Material
	bool bResult = Super::PostLoad();

	

	//fs::path rawFilePath = FileSystem::FindRawFile(FileSystem::FromString(GetInfo().GetSPath()));
	//std::string staticMeshName = rawFilePath.filename().string();

	//if (!rawFilePath.empty())
	//{
	//	AssimpLoader loader;
	//	loader.Load(this, rawFilePath.string().c_str());
	//}

	return true;
}

bool MeshAsset::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	if (ar.IsSaving())
	{		
		std::string RelPathWithName;
		GetRelativePathWithName(OUT RelPathWithName);
		fs::path vtxpath = RelPathWithName;
		vtxpath.replace_filename(GetName() + "_data.bin");
		std::string sPath = vtxpath.string();
		ar.Serialize("vtx_file_path", sPath);
		//Expression MeshContainer		
		std::vector<std::vector<SVertex>> vecCon;

		std::vector<std::string>		vecNames;
		std::vector<std::vector<uint32>> indexCon;
		

		size_t containerSize = m_MeshContainer.size();

		for(size_t i =0;i< containerSize;i++)
		{
			const SD3DBuffer& vtxBuffer = g_Engine->Get<Renderer>()->GetVertexBuffer(m_MeshContainer[i].GetIAPair().first);
			std::vector<SVertex> vertices;
			vertices.resize(vtxBuffer.Desc.ElementCount);
			::memcpy(vertices.data(), vtxBuffer.CPUData, vtxBuffer.Desc.ElementCount * vtxBuffer.Desc.Stride);
			vecCon.push_back(vertices);			
			
			size_t meshIdxPair = m_MeshContainer[i].GetIAPairSize();

			for(size_t j=0;j< meshIdxPair;j++)
			{
				const SD3DBuffer& idxBuffer = g_Engine->Get<Renderer>()->GetIndexBuffer(m_MeshContainer[i].GetIAPair((int32)j).second);
				std::vector<uint32>				indices;
				indices.resize(idxBuffer.Desc.ElementCount);
				::memcpy(indices.data(), idxBuffer.CPUData, idxBuffer.Desc.ElementCount * idxBuffer.Desc.Stride);
				indexCon.push_back(indices);
			}				

			vecNames.push_back(m_MeshContainer[i].GetMeshName());
		}			

		const SPathInfo* Pathinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		fs::path fullPath = Pathinfo->aPath;
		fullPath /= vtxpath;
		bool skeletal = false;
		if(GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
		{
			skeletal = true;
		}
		if(!FileSystem::FileExists(fullPath))
			SaveMeshVtxFile(fullPath.string().c_str(), vecCon, indexCon,vecNames, skeletal,m_CollisionData);


		FMeshToMaterialLookup::iterator iter =  m_MaterialLookupPerMesh.begin();
		FMeshToMaterialLookup::iterator iter_end = m_MaterialLookupPerMesh.end();		
		std::vector<FMeshID>		meshIDs;
		std::vector<Material*>		materials;

		for(;iter != iter_end;iter++)
		{
			meshIDs.push_back((*iter).first);
			materials.push_back((*iter).second);
		}
		ar.Serialize("meshIDs", meshIDs);
		ar.Serialize("materials", materials);
	}
	//Load Custom from
	else
	{
		std::string RelPathWithName;
		GetRelativePathWithName(OUT RelPathWithName);
		fs::path vtxpath = RelPathWithName;
		vtxpath.replace_filename(GetName() + "_data.bin");
		std::string sPath = vtxpath.string();
		ar.Serialize("vtx_file_path", sPath);

		std::vector<std::vector<SVertex>> vecCon;
		std::vector<std::vector<uint32>> indexCon;

		const SPathInfo* Pathinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		fs::path fullPath = Pathinfo->aPath;
		fullPath /= vtxpath;
		std::vector<std::string> names;
		bool skeletal = false;
		if (GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
		{
			skeletal = true;
		}
		LoadMeshVtxFile(fullPath.string().c_str(), OUT vecCon, OUT indexCon, OUT names, skeletal, OUT m_CollisionData);

		//CPU Data to GPU

		size_t containerSize = vecCon.size();
		for (size_t i = 0; i < containerSize; i++)
		{			
			Mesh newMesh = Mesh(names[i]);
			newMesh.Initialize<SVertex>(vecCon[i], indexCon[0]);

			ComputeBB(vecCon[i]);
			
			size_t perVertexBuffer = indexCon.size();
			for(size_t j=1;j<perVertexBuffer;j++)
			{
				newMesh.Initialize(newMesh.GetIAPair(0).first, indexCon[j]);
			}						
			m_MeshContainer.push_back(newMesh);
		}

		std::vector<FMeshID>		meshIDs;
		std::vector<Material*>		materials;
		ar.Serialize("meshIDs", meshIDs);
		ar.Serialize("materials", materials); 
		size_t matSizePerMesh = meshIDs.size();

		for(size_t i = 0 ;i<matSizePerMesh;i++)
		{
			m_MaterialLookupPerMesh.insert(std::make_pair(meshIDs[i], materials[i]));
		}		
	

	}

	return bResult;
}

bool MeshAsset::SaveMeshVtxFile(const char* FullPath, const std::vector<std::vector<SVertex>>& vecContainer, const std::vector<std::vector<uint32>>& vecIndexContainer, const std::vector<std::string>& Names,bool Skeletal, const SMeshCollisionData& CollisionData)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;


	size_t vecConSize = vecContainer.size();
	::fwrite(&vecConSize, sizeof(size_t), 1, File);
	for (size_t j = 0; j < vecConSize; j++)
	{
		std::vector<SVertex> vertices = vecContainer[j];
		size_t vertexSize = vertices.size();
		::fwrite(&vertexSize, sizeof(size_t), 1, File);
		for (size_t i = 0; i < vertexSize; i++)
		{
			::fwrite(&vertices[i].Position, sizeof(SVector3), 1, File);
			::fwrite(&vertices[i].Normal, sizeof(SVector3), 1, File);
			::fwrite(&vertices[i].Tangent, sizeof(SVector3), 1, File);
			::fwrite(&vertices[i].UV, sizeof(SVector2), 1, File);
			if (Skeletal)
			{
				::fwrite(&vertices[i].BlendWeight, sizeof(SVector4), 1, File);
				::fwrite(&vertices[i].BlendIndex, sizeof(SVector4), 1, File);
			}
		}

		size_t idxPerVtxSize = vecIndexContainer.size();
		::fwrite(&idxPerVtxSize, sizeof(size_t), 1, File);
		for(size_t q = 0;q< idxPerVtxSize;q++)
		{
			std::vector<uint32> indices = vecIndexContainer[q];
			size_t indexSize = indices.size();
			::fwrite(&indexSize, sizeof(size_t), 1, File);
			for (size_t i = 0; i < indexSize; i++)
			{
				::fwrite(&indices[i], sizeof(uint32), 1, File);
			}			
		}		
		
		size_t nameSize = Names.size();
		::fwrite(&nameSize, sizeof(size_t), 1, File);
		for (size_t i = 0; i < nameSize; i++)
		{
			//Len
			size_t length = Names[i].length() +1;
			::fwrite(&length, sizeof(size_t), 1, File);
			::fwrite(Names[i].data(), sizeof(char) * length, 1, File);
		}
	}


	//Save Collision
	size_t collisionVtxSize = CollisionData.vertexData.size();
	::fwrite(&collisionVtxSize, sizeof(size_t), 1, File);
	for (size_t j = 0; j < collisionVtxSize; j++)
	{
		::fwrite(&CollisionData.vertexData[j], sizeof(SVector3), 1, File);		
	}
	size_t collisionIdxSize = CollisionData.indexData.size();
	::fwrite(&collisionIdxSize, sizeof(size_t), 1, File);
	for (size_t j = 0; j < collisionIdxSize; j++)
	{
		::fwrite(&CollisionData.indexData[j], sizeof(uint32), 1, File);
	}


	fclose(File);

	return true;
}

bool MeshAsset::LoadMeshVtxFile(const char* FullPath, std::vector<std::vector<SVertex>>& OutContainer, std::vector<std::vector<uint32>>& OutIndex, std::vector<std::string>& OutNames,bool Skeletal, SMeshCollisionData& OutCollisionData)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;


	size_t vecConSize = 0;
	::fread(&vecConSize, sizeof(size_t), 1, File);
	OutContainer.resize(vecConSize);

	for (size_t j = 0; j < vecConSize; j++)
	{
		std::vector<SVertex> vertices;
		size_t vertexSize = 0;
		::fread(&vertexSize, sizeof(size_t), 1, File);
		vertices.resize(vertexSize);
		for (size_t i = 0; i < vertexSize; i++)
		{
			::fread(&vertices[i].Position, sizeof(SVector3), 1, File);
			::fread(&vertices[i].Normal, sizeof(SVector3), 1, File);
			::fread(&vertices[i].Tangent, sizeof(SVector3), 1, File);
			::fread(&vertices[i].UV, sizeof(SVector2), 1, File);
			if (Skeletal)
			{
				::fread(&vertices[i].BlendWeight, sizeof(SVector4), 1, File);
				::fread(&vertices[i].BlendIndex, sizeof(SVector4), 1, File);
			}

		}
		OutContainer[j] = vertices;

		size_t idxPerVtxSize = 0;
		::fread(&idxPerVtxSize, sizeof(size_t), 1, File);
		OutIndex.resize(idxPerVtxSize);
		for(size_t q = 0;q<idxPerVtxSize;q++)
		{

			std::vector<uint32> indices;// = vecIndexContainer[j];
			size_t indexSize = 0;
			::fread(&indexSize, sizeof(size_t), 1, File);
			indices.resize(indexSize);

			for (size_t i = 0; i < indexSize; i++)
			{
				::fread(&indices[i], sizeof(uint32), 1, File);
			}
			OutIndex[q] = indices;
		}


		size_t nameSize = 0;
		::fread(&nameSize, sizeof(size_t), 1, File);
		OutNames.resize(nameSize);
		for (size_t i = 0; i < nameSize; i++)
		{
			size_t length = 0;
			::fread(&length, sizeof(size_t), 1, File);
			if (length)
			{
				char* loadName = new char[length];
				::fread(loadName, sizeof(char) * length, 1, File);
				OutNames[i] = loadName;
				SAFE_DELETE(loadName);
			}
		}
	}

	//Save Collision
	size_t collisionVtxSize =0;
	::fread(&collisionVtxSize, sizeof(size_t), 1, File);
	OutCollisionData.vertexData.resize(collisionVtxSize);
	for (size_t j = 0; j < collisionVtxSize; j++)
	{
		::fread(&OutCollisionData.vertexData[j], sizeof(SVector3), 1, File);
	}
	size_t collisionIdxSize = 0;	
	::fread(&collisionIdxSize, sizeof(size_t), 1, File);
	OutCollisionData.indexData.resize(collisionIdxSize);
	for (size_t j = 0; j < collisionIdxSize; j++)
	{
		::fread(&OutCollisionData.indexData[j], sizeof(uint32), 1, File);
	}


	fclose(File);

	return true;
}

void MeshAsset::AddRenderCount(bool Add)
{
	if (Add)
		++m_RenderCount;
	else
		m_RenderCount = std::max(0, m_RenderCount-1);
}

int32 MeshAsset::AddMesh(const Mesh& InMesh)
{
	int32 meshSize = (int32)m_MeshContainer.size();
	
	m_MeshContainer.push_back(InMesh);
	return meshSize;

}

void MeshAsset::AddMaterial(int32 SlotIdx, class Material* InMaterial)
{
	FMeshToMaterialLookup::const_iterator iter = m_MaterialLookupPerMesh.find(SlotIdx);
	
	m_MaterialLookupPerMesh[SlotIdx] = InMaterial;
}

void MeshAsset::ComputeBB(const std::vector<SVertex>& InVertices)
{
	size_t vertexSize = InVertices.size();
	for(size_t i=0;i<vertexSize;i++)
	{		
		m_BBMin.x = std::min(InVertices[i].Position.x, m_BBMin.x);
		m_BBMin.y = std::min(InVertices[i].Position.y, m_BBMin.y);
		m_BBMin.z = std::min(InVertices[i].Position.z, m_BBMin.z);

		m_BBMax.x = std::max(InVertices[i].Position.x, m_BBMax.x);
		m_BBMax.y = std::max(InVertices[i].Position.y, m_BBMax.y);
		m_BBMax.z = std::max(InVertices[i].Position.z, m_BBMax.z);
		
	}
}

void MeshAsset::AddCollisionMeshData(const std::vector<SVector3>& vertices, const std::vector<uint32>& indices)
{
	m_CollisionData.vertexData.resize(vertices.size());
	size_t vertexSize =  m_CollisionData.vertexData.size();
	for(size_t v=0;v < vertexSize;v++)
	{
		m_CollisionData.vertexData[v] = vertices[v];
	}

	m_CollisionData.indexData.resize(indices.size());
	size_t indexSize = m_CollisionData.indexData.size();
	for (size_t i = 0; i < indexSize; i++)
	{
		m_CollisionData.indexData[i] = indices[i];
	}
}

const SMeshCollisionData& MeshAsset::GetMeshCollisionData() const
{
	return m_CollisionData;
}

void MeshAsset::GetAABB(SVector3& outBBMin, SVector3& outBBMax)
{
	outBBMax = m_BBMax;
	outBBMin = m_BBMin;
}
