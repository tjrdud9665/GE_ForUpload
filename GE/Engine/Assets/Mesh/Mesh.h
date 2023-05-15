#pragma once

#include "../AssetData.h"
#include "../../Renderer/Renderer.h"
#include "../../Engine.h"
#include "../Material/Material.h"




class Mesh
{
	friend class Renderer;
	friend class GeometryGenerator;
	friend class RenderManager;
	friend class AssimpLoader;
	friend class MeshAsset;
	friend class FBXLoader;
	friend class Widget;
	friend class PanelSlot;
	friend class BillboardComponent;

public:
	Mesh(const std::string& InMeshName ="");
	virtual ~Mesh();	

private:
	template<class VertexBufferType>
	bool Initialize(const std::vector<VertexBufferType>& vertices,const std::vector<uint32>& indices);
	
	bool Initialize(FBufferID vertexID, const std::vector<uint32>& indices);

	template<class VertexBufferType>
	bool InitializeDynamic(const std::vector<VertexBufferType>& vertices, const std::vector<uint32>& indices);

public:
	FIAPair GetIAPair(int32 Idx = 0) const
	{
		return m_IABuffer[Idx];
	}
	size_t GetIAPairSize() const
	{
		return m_IABuffer.size();
	}
	const std::string& GetMeshName() const
	{
		return m_MeshName;
	}
private:
	std::vector<FIAPair>		m_IABuffer;
	std::string					m_MeshName;

	SVector3					m_BBMin;
	SVector3					m_BBMax;
};

template<class VertexBufferType>
bool Mesh::Initialize(const std::vector<VertexBufferType>& vertices, const std::vector<uint32>& indices)
{
	Renderer* pRenderer = g_Engine->Get<Renderer>();

	if (!pRenderer)
	{
		return false;
	}

	SBufferDesc bufferDesc = {};
	
	const std::string VBName = m_MeshName + "_LOD[0]_VB";
	const std::string IBName = m_MeshName + "_LOD[0]_IB";

	bufferDesc.BufferType = EBufferType::VERTEX_BUFFER;
	bufferDesc.Usage = EBufferUsage::GPU_READ;
	bufferDesc.ElementCount = static_cast<uint32>(vertices.size());
	bufferDesc.Stride = sizeof(vertices[0]);
	FBufferID vertexBufferID = pRenderer->CreateBuffer(bufferDesc, vertices.data(), VBName.c_str());

	

	bufferDesc.BufferType = EBufferType::INDEX_BUFFER;
	bufferDesc.Usage = EBufferUsage::GPU_READ;
	bufferDesc.ElementCount = static_cast<uint32>(indices.size());
	bufferDesc.Stride = sizeof(uint32);																/// Index 
	FBufferID indexBufferID = pRenderer->CreateBuffer(bufferDesc, indices.data(), IBName.c_str());

	
	
	m_IABuffer.push_back(std::make_pair(vertexBufferID, indexBufferID));

	return true;
}

template<class VertexBufferType>
bool Mesh::InitializeDynamic(const std::vector<VertexBufferType>& vertices, const std::vector<uint32>& indices)
{
	Renderer* pRenderer = g_Engine->Get<Renderer>();

	if (!pRenderer)
	{
		return false;
	}

	SBufferDesc bufferDesc = {};

	const std::string VBName = m_MeshName + "_LOD[0]_VB";
	const std::string IBName = m_MeshName + "_LOD[0]_IB";

	bufferDesc.BufferType = EBufferType::VERTEX_BUFFER;
	bufferDesc.Usage = EBufferUsage::GPU_READ_CPU_WRITE;
	bufferDesc.ElementCount = static_cast<uint32>(vertices.size());
	bufferDesc.Stride = sizeof(vertices[0]);
	bufferDesc.CPUAccess = ECPUAccess::CPU_W;
	FBufferID vertexBufferID = pRenderer->CreateBuffer(bufferDesc, vertices.data(), VBName.c_str());



	bufferDesc.BufferType = EBufferType::INDEX_BUFFER;
	bufferDesc.Usage = EBufferUsage::GPU_READ;
	bufferDesc.ElementCount = static_cast<uint32>(indices.size());
	bufferDesc.Stride = sizeof(uint32);																/// Index 
	bufferDesc.CPUAccess = ECPUAccess::NONE;
	FBufferID indexBufferID = pRenderer->CreateBuffer(bufferDesc, indices.data(), IBName.c_str());



	m_IABuffer.push_back(std::make_pair(vertexBufferID, indexBufferID));

	return true;
}

