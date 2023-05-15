#include "PCH.h"
#include "MonsterSpawnObject.h"
#include "ObjectManager.h"
#include "GameFramework/Components/BoxComponent.h"
#include "Assets/Scene/Scene.h"
#include "SceneManager.h"
#include "Assets/SceneInfo/SceneInfo.h"
#include "../SceneInfo/CustomSceneInfo.h"


MonsterSpawnObject::MonsterSpawnObject()
{

}

MonsterSpawnObject::MonsterSpawnObject(const MonsterSpawnObject& Other)
	: Super(Other)	
	, m_CustomSceneInfo(Other.m_CustomSceneInfo)
{
	m_SpawnRoot = FindComponent<BoxComponent>("SpawnBox");

	size_t MonsterCount = Other.m_Monster.size();
	m_Monster.resize(MonsterCount);

	for (size_t i = 0; i < MonsterCount; ++i)
	{
		m_Monster[i] = Other.m_Monster[i];
	}

}

MonsterSpawnObject::~MonsterSpawnObject()
{

}

void MonsterSpawnObject::BeginPlay()
{
	Super::BeginPlay();	

	m_SpawnPosition = GetWorldLocation();
	m_SpawnScale = GetWorldScale3D();
}

void MonsterSpawnObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_SpawnRoot = CreateComponent<BoxComponent>("SpawnBox");
	SetRootComponent(m_SpawnRoot);

}

void MonsterSpawnObject::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	m_DeltaTime += DeltaSeconds;

	if (m_IsBegun)
	{
		//최초 씬 진입시에는 차근차근 소환할 필요가 없음 . 미리 몬스터가 배치되어있어야 함 .
		//MaxCount만큼 미리 배치 .

		if (!m_GuardianSpawn)
		{
			if (!m_StartDungeon)
			{
				StartSpawn();
			}

			//이후 스폰 갱신

			else if (m_DeltaTime > m_RespawnTime)
			{
				SpawnUpdate();
			}
		}

		if (m_GuardianSpawn)
		{
			if (m_DungeonClear)
			{
				m_GuardianSpawnTime += DeltaSeconds;

				if (m_GuardianSpawnTime > 3.f)
				{
					GuardianSpawn();

					m_GuardianSpawn = false;

					m_GuardianSpawnTime = 0.f;
				}			
			}

		}
	}


}

void MonsterSpawnObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (!m_Start && m_IsBegun)	
		AddSceneInfo();			

	// 중간에 죽은 몬스터가 있을 경우 찾아서 삭제후 카운트 갱신 , 씬인포로 킬카운트 전달
	MonsterDieCheck();
}


void MonsterSpawnObject::SetMonsterClear()
{
	size_t Size = m_vecMonster.size();

	for (int i = 0; i < Size; ++i)
	{
		m_vecMonster[i]->Destroy();	
	}
}

void MonsterSpawnObject::SetDungeonClear()
{
	m_DungeonClear = true;
}

void MonsterSpawnObject::GuardianSpawn()
{
	BossMonster* GuardianObj = nullptr;

	GuardianObj = g_Engine->Get<ObjectManager>()->CreateGameObject<BossMonster>(m_Guardian[0]);

	if (GuardianObj)
	{
		GuardianObj->SetWorldLocation(SVector3(m_SpawnPosition.x, m_SpawnPosition.y + GuardianObj->GetCapsuleHalfLength(), m_SpawnPosition.z));

		g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(GuardianObj);
		
	}

}

void MonsterSpawnObject::MonsterDieCheck()
{
	auto iter = m_vecMonster.begin();
	auto iterEnd = m_vecMonster.end();

	for (; iter != iterEnd; )
	{
		if (0 >= (*iter)->GetRefCount())
		{
			iter = m_vecMonster.erase(iter);
			iterEnd = m_vecMonster.end();

			//m_CustomSceneInfo->AddChaosDungeonKillCount();

			--m_CurrentCount;

			continue;
		}
		++iter;
	}
}

void MonsterSpawnObject::StartSpawn()
{
	if (m_CurrentCount == m_MaxMonsterCount)
		m_StartDungeon = true;

	if (m_MaxMonsterCount > m_CurrentCount)
	{
		float MaxSpawnPosX = m_SpawnPosition.x + (m_SpawnScale.x / 2.f);
		float MaxSpawnPosZ = m_SpawnPosition.z + (m_SpawnScale.z / 2.f);

		float RandomPosX = SMathUtils::Random(m_SpawnPosition.x - (m_SpawnScale.x / 2.f), MaxSpawnPosX);
		float RandomPosZ = SMathUtils::Random(m_SpawnPosition.z - (m_SpawnScale.z / 2.f), MaxSpawnPosZ);

		Monster* MonsterObj = nullptr;

		if (m_Monster.size() > 0)
		{
			m_MonsterCount = m_CurrentCount % (int)m_Monster.size();

			if (m_MonsterCount == 0)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[0]);
			}

			else if (m_MonsterCount == 1)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[1]);
			}

			else if (m_MonsterCount == 2)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[2]);
			}

			else if (m_MonsterCount == 3)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[3]);
			}


			if (MonsterObj)
			{
		
				MonsterObj->SetWorldLocation(SVector3(RandomPosX, m_SpawnPosition.y + MonsterObj->GetCapsuleHalfLength(), RandomPosZ));

				g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(MonsterObj);		
			

				m_DeltaTime = 0.f;

				++m_CurrentCount;

				m_vecMonster.push_back(MonsterObj);
			}
		}
	}
}

void MonsterSpawnObject::SpawnUpdate()
{
	if (m_MaxMonsterCount > m_CurrentCount && !m_DungeonClear)
	{
		float MaxSpawnPosX = m_SpawnPosition.x + (m_SpawnScale.x / 2.f);
		float MaxSpawnPosZ = m_SpawnPosition.z + (m_SpawnScale.z / 2.f);

		float RandomPosX = SMathUtils::Random(m_SpawnPosition.x - (m_SpawnScale.x / 2.f), MaxSpawnPosX);
		float RandomPosZ = SMathUtils::Random(m_SpawnPosition.z - (m_SpawnScale.z / 2.f), MaxSpawnPosZ);
	

		Monster* MonsterObj = nullptr;
		if (m_Monster.size() > 0)
		{
			m_MonsterCount = m_CurrentCount % (int)m_Monster.size();

			if (m_MonsterCount == 0)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[0]);
			}

			else if (m_MonsterCount == 1)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[1]);
			}

			else if (m_MonsterCount == 2)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[2]);
			}

			else if (m_MonsterCount == 3)
			{
				MonsterObj = g_Engine->Get<ObjectManager>()->CreateGameObject<Monster>(m_Monster[3]);
			}

			if (MonsterObj)
			{
				MonsterObj->SetWorldLocation(SVector3(RandomPosX, m_SpawnPosition.y + MonsterObj->GetCapsuleHalfLength(), RandomPosZ));

				g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(MonsterObj);

				m_DeltaTime = 0.f;

				++m_CurrentCount;

				m_vecMonster.push_back(MonsterObj);
			}
		}
	}
}

void MonsterSpawnObject::AddSceneInfo()
{
		m_CustomSceneInfo = (CustomSceneInfo*)g_Engine->Get<SceneManager>()->GetCurrentScene()->GetSceneInfo();

		if (m_CustomSceneInfo)
		{
			if (!m_GuardianSpawn)
				m_CustomSceneInfo->AddSpawn(this);

			else
				m_CustomSceneInfo->AddGuardianSpawn(this);

			m_Start = true;
		}	
}

