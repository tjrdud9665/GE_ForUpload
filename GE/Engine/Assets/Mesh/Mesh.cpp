#include "EnginePCH.h"
#include "Mesh.h"
#include "Renderer/Renderer.h"
#include "Importer/FBXLoader.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/D3DBuffer.h"
#include "../../AssetManager.h"
#include "../TextureAsset/TextureAsset.h"
#include "../../Path/PathManager.h"

Mesh::Mesh(const std::string& InMeshName)
	:m_MeshName(InMeshName)
{
	//m_IABuffer = std::make_pair(INDEX_NONE, INDEX_NONE);
	
}


Mesh::~Mesh()
{

}

bool Mesh::Initialize(FBufferID vertexID, const std::vector<uint32>& indices)
{
	Renderer* pRenderer = g_Engine->Get<Renderer>();

	if (!pRenderer)
	{
		return false;

	}
	SBufferDesc bufferDesc = {};

	const std::string IBName = m_MeshName + "_LOD[0]_IB";

	bufferDesc.BufferType = EBufferType::INDEX_BUFFER;
	bufferDesc.Usage = EBufferUsage::GPU_READ;
	bufferDesc.ElementCount = static_cast<uint32>(indices.size());
	bufferDesc.Stride = sizeof(uint32);																/// Index 
	FBufferID indexBufferID = pRenderer->CreateBuffer(bufferDesc, indices.data(), IBName.c_str());

	m_IABuffer.push_back(std::make_pair(vertexID, indexBufferID));

	return true;
}
