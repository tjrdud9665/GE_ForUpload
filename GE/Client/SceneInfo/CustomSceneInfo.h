#pragma once

#include "Assets/SceneInfo/SceneInfo.h"


class CustomSceneInfo : public SceneInfo
{
	CLASS(CustomSceneInfo, SceneInfo)
		friend class Scene;

protected:
	CustomSceneInfo();
	CustomSceneInfo(const CustomSceneInfo& Other);
	virtual ~CustomSceneInfo();

protected:
	virtual void BeginPlay();

	virtual void PreUpdate(float DeltaSeconds);

	virtual void Update(float DeltaSeconds);

	virtual void PostUpdate(float DeltaSeconds);

protected:
	std::vector<class MonsterSpawnObject*>	m_vecSpawn;
	std::vector<class MonsterSpawnObject*>	m_vecGuardianSpawn;
	int32		m_CurentKillCount;			// ���� ���� Kill Count
	float		m_ChaosDungeonPercent;		// ī�������� �����  
	bool		m_ChaosDungeonClear;		// ī�������� Ŭ���� ���� 

	PROP(int32, m_MaxKillCount, DetailsEdit)
	int32	m_MaxKillCount;


	PROP(TSubclassOf<UserWidget>, m_DungeonInfoWidgetClass, DetailsEdit)
	TSubclassOf<UserWidget>		m_DungeonInfoWidgetClass;

public:
	int32 GetCurrontMonsterCount() const;
	int32 GetRemainMonsterCount() const;
	float GetPercent() const;

public:
	int32 GetGuardianCount() const;
	void AddChaosDungeonKillCount();
	void AddSpawn(class MonsterSpawnObject* Spawn);
	void AddGuardianSpawn(class MonsterSpawnObject* Spawn);
	void DungeonClear();
};
