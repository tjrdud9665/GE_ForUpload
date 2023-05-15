#include "EnginePCH.h"
#include "StaticMeshComponent.h"
#include <RenderManager.h>
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include <Renderer/Renderer.h>
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"


StaticMeshComponent::StaticMeshComponent()
	:StaticMesh(nullptr)
{

}

StaticMeshComponent::StaticMeshComponent(const StaticMeshComponent& Other)
	:Super(Other)
	,StaticMesh(Other.StaticMesh)
{	
	if (GetMeshAsset())
		StaticMesh = GetMeshAsset();
	else
		SetMeshAsset(StaticMesh);
}

StaticMeshComponent::~StaticMeshComponent()
{
	SAFE_DELETE(m_meshInterface);
}

void StaticMeshComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
}

void StaticMeshComponent::Render(class Renderer* pRenderer)
{	
	pRenderer->SetConstant1i("UseSkinning", false);
	Super::Render(pRenderer);
	
}

void StaticMeshComponent::RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj)
{
	pRenderer->SetConstant1i("UseSkinning", false);
	Super::RenderDepth(pRenderer, ViewProj);
}

void StaticMeshComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	SetMeshAsset(StaticMesh);	
}

void StaticMeshComponent::GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID)
{
	Super::GatherRenderInstance(pRenderer, InstanceInfos, InstanceID);

}

void StaticMeshComponent::SetMeshAsset(class MeshAsset* InMeshAsset)
{
	Super::SetMeshAsset(InMeshAsset);

	StaticMesh = InMeshAsset;
}

void StaticMeshComponent::CreateShape()
{	
	if (!GetMeshAsset())
	{
		return;
	}
	Renderer* renderer =  g_Engine->Get<Renderer>();
	if(!renderer)
	{
		return;
	}

	if (m_DirtyShape)
	{
		SAFE_DELETE(m_Shape);
	}	
	if (m_Shape)
	{
		return;
	}
	const SMeshCollisionData& collisionData = GetMeshAsset()->GetMeshCollisionData();

	bool HasCollisionData = collisionData.vertexData.size() != 0;

	SAFE_DELETE(m_meshInterface);
	m_meshInterface = new btTriangleIndexVertexArray();
	if(HasCollisionData)
	{
		btIndexedMesh indexedMesh;
		indexedMesh.m_indexType = PHY_ScalarType::PHY_INTEGER;
		indexedMesh.m_numTriangles = (int32)collisionData.indexData.size() / 3;

		indexedMesh.m_numVertices = (int32)collisionData.vertexData.size();
		indexedMesh.m_triangleIndexBase = (const unsigned char*)collisionData.indexData.data();
		indexedMesh.m_triangleIndexStride = sizeof(uint32) * 3;

		indexedMesh.m_vertexBase = (const unsigned char*)collisionData.vertexData.data();
		indexedMesh.m_vertexStride = sizeof(float) * 3;
		indexedMesh.m_vertexType = PHY_FLOAT;
		m_meshInterface->addIndexedMesh(indexedMesh);
		m_Shape = new btBvhTriangleMeshShape(m_meshInterface, true);
		m_DirtyShape = false;
	}
	else
	{
		const SD3DBuffer& vtxBuffer = renderer->GetVertexBuffer(GetMeshAsset()->GetMesh(0).GetIAPair().first);
		const SD3DBuffer& idxBuffer = renderer->GetIndexBuffer(GetMeshAsset()->GetMesh(0).GetIAPair().second);

		std::vector<SVertex> vertices;
		vertices.resize(vtxBuffer.Desc.ElementCount);
		::memcpy(vertices.data(), vtxBuffer.CPUData, vtxBuffer.Desc.ElementCount * vtxBuffer.Desc.Stride);


		m_Shape = new btConvexHullShape(
			(btScalar*)&vertices[0],                                 // points
			(int32)vertices.size(),									     // point count
			static_cast<uint32>(sizeof(SVertex)));						// stride
		m_DirtyShape = false;

		auto hull = static_cast<btConvexHullShape*>(m_Shape);
	}


	/*
	SAFE_DELETE(m_meshInterface);
		m_meshInterface = new btTriangleIndexVertexArray();
		//uint32

		// Create a btIndexedMesh object for the current mesh part.
		btIndexedMesh indexedMesh;
		indexedMesh.m_indexType = PHY_ScalarType::PHY_INTEGER;
		indexedMesh.m_numTriangles = indices.size() / 3;
		indexedMesh.m_numVertices = indices.size();
		indexedMesh.m_triangleIndexBase = (const unsigned char*)m_meshData.indexData.data();
		indexedMesh.m_triangleIndexStride = sizeof(uint32) * 3;

		indexedMesh.m_vertexBase = (const unsigned char*)m_meshData.vertexData.data();
		indexedMesh.m_vertexStride = sizeof(float) * 3;
		indexedMesh.m_vertexType = PHY_FLOAT;


		m_meshInterface->addIndexedMesh(indexedMesh);

		m_Shape = new btBvhTriangleMeshShape(m_meshInterface,true);
		
	*/


	

	//hull->optimizeConvexHull();

	

	
}
