#pragma once
#include "EditorWindow.h"

//SceneViewport �� ������Ʈ�� �߰� �� �� �ֵ��� ���������� �ǰ��� ObjectList �� Spawn

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

