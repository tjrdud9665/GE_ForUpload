#pragma once
#include "EditorWindow.h"

//SceneViewport 에 오브젝트를 추가 할 수 있도록 계층구조에 의거한 ObjectList 및 Spawn

class ObjectPlacement :public EditorWindow
{
	CLASS(ObjectPlacement,EditorWindow)

protected:
	ObjectPlacement();
	ObjectPlacement(const ObjectPlacement& Other);
	virtual ~ObjectPlacement();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Build() override;	

	virtual void Update(float DeltaSeconds) override;

private:
	void ShowSpawnButton();

private:
	class ObjectListWidget* m_ObjectLists;

	std::string				m_SpawnObjectName;
	SVector3				m_SpawnLocation;
	SVector3				m_SpawnRotation;
	SVector3				m_SpawnScale;
};

