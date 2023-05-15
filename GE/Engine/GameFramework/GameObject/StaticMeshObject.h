#pragma once
#include "GameObject.h"

class StaticMeshObject :public GameObject
{
	CLASS(StaticMeshObject,GameObject)

protected:
	StaticMeshObject();
	StaticMeshObject(const StaticMeshObject& Other);
	virtual ~StaticMeshObject();

	virtual void Construct(const std::string& InName /* = "" */) override;

private:
	class StaticMeshComponent* m_MeshComponent;


};

