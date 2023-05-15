#include "EnginePCH.h"
#include "StaticMeshObject.h"
#include "../Components/StaticMeshComponent.h"

StaticMeshObject::StaticMeshObject()
{

}

StaticMeshObject::StaticMeshObject(const StaticMeshObject& Other)
	:Super(Other)	
{
	m_MeshComponent = FindComponent<StaticMeshComponent>("StaticMesh");
}

StaticMeshObject::~StaticMeshObject()
{

}

void StaticMeshObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	
	m_MeshComponent = CreateComponent<StaticMeshComponent>("StaticMesh");

	SetRootComponent(m_MeshComponent);
}
