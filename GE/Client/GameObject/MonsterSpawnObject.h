#pragma once
#include <GameFramework\GameObject\GameObject.h>
#include "Monster.h"
#include "BossMonster.h"

class MonsterSpawnObject : public GameObject
{
	CLASS(MonsterSpawnObject, GameObject)

protected:
	MonsterSpawnObject();
	MonsterSpawnObject(const MonsterSpawnObject& Other);
	virtual ~MonsterSpawnObject();

public:
	virtual void BeginPlay() override;

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void Update(float DeltaSeconds) override;


protected:
	class BoxComponent* m_SpawnRoot;
	class CustomSceneInfo* m_CustomSceneInfo;
	std::vector<class Monster*>	m_vecMonster;
	float	m_DeltaTime;
	int32	m_CurrentCount;
	int32	m_MonsterCount;
	bool	m_StartDungeon;
	bool	m_DungeonClear;
	bool	m_Start;

	//가디언용
	float			m_GuardianSpawnTime;

	SVector3		m_SpawnPosition;
	SVector3		m_SpawnScale;

	PROP(std::vector<TSubclassOf<Monster>>, m_Monster, DetailsEdit)
	std::vector<TSubclassOf<Monster>>	m_Monster;	

	PROP(std::vector<TSubclassOf<BossMonster>>, m_Guardian, DetailsEdit)
	std::vector<TSubclassOf<BossMonster>>  m_Guardian;

	PROP(int32, m_MaxMonsterCount, DetailsEdit)
	int32 m_MaxMonsterCount;

	PROP(bool, m_GuardianSpawn, DetailsEdit)
	bool m_GuardianSpawn;

	PROP(float, m_RespawnTime, DetailsEdit)
	float m_RespawnTime;

public:
	void SetMonsterClear();
	void SetDungeonClear();
	void GuardianSpawn();

private:
	void MonsterDieCheck();
	void StartSpawn();
	void SpawnUpdate();
	void AddSceneInfo();
};

