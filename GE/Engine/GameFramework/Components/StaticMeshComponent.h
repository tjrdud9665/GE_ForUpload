#pragma once
#include "PrimitiveComponent.h"

struct SMeshData
{
	std::vector<SVector3> vertexData;
	//float* vertexData;
	std::vector<uint32> indexData;
};

class StaticMeshComponent :public PrimitiveComponent
{
	CLASS(StaticMeshComponent, PrimitiveComponent)

	friend class PhysicsSystem;

protected:
	StaticMeshComponent();
	StaticMeshComponent(const StaticMeshComponent& Other);
	virtual ~StaticMeshComponent();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Render(class Renderer* pRenderer) override;

	virtual void RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID) override;

	virtual void SetMeshAsset(class MeshAsset* InMeshAsset) override;

	virtual void CreateShape()  override;

private:
	PROP(class MeshAsset*, StaticMesh, DetailsEdit , "Primitive")
	class MeshAsset* StaticMesh;	
	
};

