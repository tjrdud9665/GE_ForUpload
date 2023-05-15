#include "PCH.h"
#include "CustomSceneInfo.h"
#include "Assets/SceneInfo/SceneInfo.h"
#include "Assets/Scene/Scene.h"
#include "../GameObject/MonsterSpawnObject.h"
#include "../GameObject/Monster.h"
#include "Assets/Widget/UserWidget.h"
#include "Engine.h"
#include "SceneViewport.h"
#include "../GameObject/TeleportObject.h"
#include "ObjectManager.h"

CustomSceneInfo::CustomSceneInfo()
	:m_ChaosDungeonPercent(0.f)
{

}

CustomSceneInfo::CustomSceneInfo(const CustomSceneInfo& Other)
	: Super(Other)
	, m_MaxKillCount(Other.m_MaxKillCount)
	, m_CurentKillCount(Other.m_CurentKillCount)
	, m_ChaosDungeonClear(Other.m_ChaosDungeonClear)
	, m_ChaosDungeonPercent(Other.m_ChaosDungeonPercent)
	, m_DungeonInfoWidgetClass(Other.m_DungeonInfoWidgetClass)
{

}

CustomSceneInfo::~CustomSceneInfo()
{

}

void CustomSceneInfo::BeginPlay()
{
	Super::BeginPlay();

	if (!m_DungeonInfoWidgetClass.CustomCDOName.empty())
		g_Engine->Get<SceneViewport>()->CreateUserWidget<UserWidget>(m_DungeonInfoWidgetClass);
}

void CustomSceneInfo::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
}

void CustomSceneInfo::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void CustomSceneInfo::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	if (m_MaxKillCount > 0)
		m_ChaosDungeonPercent = (m_CurentKillCount / (float)m_MaxKillCount) * 100.f;

	if (m_ChaosDungeonPercent >= 100.f)
	{
		m_ChaosDungeonClear = true;
		m_ChaosDungeonPercent = 100.f;
	}

	if (m_ChaosDungeonClear)
	{
		DungeonClear();
	}
}

int32 CustomSceneInfo::GetCurrontMonsterCount() const
{
	return m_CurentKillCount;
}

int32 CustomSceneInfo::GetRemainMonsterCount() const
{
	return m_MaxKillCount - m_CurentKillCount;
}

float CustomSceneInfo::GetPercent() const
{
	return m_ChaosDungeonPercent;
}

int32 CustomSceneInfo::GetGuardianCount() const
{
	return (int32)m_vecGuardianSpawn.size();
}

void CustomSceneInfo::AddChaosDungeonKillCount()
{
	if (!m_ChaosDungeonClear)
		++m_CurentKillCount;
}

void CustomSceneInfo::AddSpawn(class MonsterSpawnObject* Spawn)
{
	m_vecSpawn.push_back(Spawn);
}

void CustomSceneInfo::AddGuardianSpawn(class MonsterSpawnObject* Spawn)
{
	m_vecGuardianSpawn.push_back(Spawn);
}

void CustomSceneInfo::DungeonClear()
{
	size_t Size = m_vecSpawn.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecSpawn[i]->SetDungeonClear();
		m_vecSpawn[i]->SetMonsterClear();
	}

	Size = m_vecGuardianSpawn.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecGuardianSpawn[i]->SetDungeonClear();
	}

	std::vector<TeleportObject*> TeleportObjects;
	size_t TeleportCount = GetOwner<Scene>()->GetObjectManager()->FindGameObjectsByClass<TeleportObject>(TeleportObjects);

	for (size_t i = 0; i < TeleportCount; i++)
	{
		TeleportObjects[i]->ActivateTeleport();

	}
}
