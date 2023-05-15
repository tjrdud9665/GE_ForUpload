#include "EnginePCH.h"
#include "SceneInfo.h"
#include <ObjectManager.h>
#include <GameFramework/GameObject/PlayerStart.h>
#include <SceneViewport.h>

SceneInfo::SceneInfo()
	:m_PlayerCharacter(nullptr)
	, m_PlayerController(nullptr)
{

}

SceneInfo::SceneInfo(const SceneInfo& Other)
	:Super(Other)	
	, m_DefaultCharacterClass(Other.m_DefaultCharacterClass)
	, m_DefaultControllerClass(Other.m_DefaultControllerClass)
	, m_DefaultWidgetClass(Other.m_DefaultWidgetClass)
	, m_PlayerCharacter(nullptr)
	, m_PlayerController(nullptr)	
{

}

SceneInfo::~SceneInfo()
{
	if (!m_OwningScene)
	{
		std::string sPath = GetInfo().GetSPath();
		if (!sPath.empty())
		{
			std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(sPath)) + GetName();
			GetClass()->DeleteCDO(CDOName);
		}
	}
	m_PlayerCharacter = nullptr;
	m_PlayerController = nullptr;
}

bool SceneInfo::PostLoad()
{
	bool Result = Super::PostLoad();

	std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(GetInfo().GetSPath())) + GetName();

	GetClass()->RegisterCDO(CDOName, this, SCENEINFO_PATH);

	return Result;
}

void SceneInfo::BeginPlay()
{
	if (!m_DefaultCharacterClass.CustomCDOName.empty())
	{
		m_PlayerCharacter = g_Engine->Get<ObjectManager>()->SpawnGameObject<Character>(m_DefaultCharacterClass->GetName(), m_DefaultCharacterClass);

		m_PlayerController = g_Engine->Get<ObjectManager>()->SpawnGameObject<PlayerController>(m_DefaultControllerClass->GetName(), m_DefaultControllerClass);

		m_PlayerController->Possess(m_PlayerCharacter);	
		
		
		m_PlayerCharacter->SetWorldLocation(GetPlayerStartLocation());
		
	}


	//Create UI	
	if(!m_DefaultWidgetClass.CustomCDOName.empty())
		g_Engine->Get<SceneViewport>()->CreateUserWidget<UserWidget>(m_DefaultWidgetClass);
}

void SceneInfo::EndPlay()
{

}

void SceneInfo::PreUpdate(float DeltaSeconds)
{

}

void SceneInfo::Update(float DeltaSeconds)
{

}

void SceneInfo::PostUpdate(float DeltaSeconds)
{
	if (m_PlayerCharacter)
	{
		if (!m_PlayerCharacter->IsValidate())
		{
			m_PlayerCharacter = nullptr;
		}
	}
}

Character* SceneInfo::GetPlayerCharacter() const
{
	return m_PlayerCharacter;
}

PlayerController* SceneInfo::GetPlayerController() const
{
	return m_PlayerController;
}

SVector3 SceneInfo::GetPlayerStartLocation() const
{
	PlayerStart* start = g_Engine->Get<ObjectManager>()->FindGameObjectByClass<PlayerStart>();
	if (start)
	{
		start->LocationUpdate();
		return (start->GetWorldLocation());
	}

	return SVector3::ZeroVector;
}

void SceneInfo::SetOwner(XObject* InOwner)
{
	Super::SetOwner(InOwner);
	if (InOwner)
	{
		if (InOwner->GetClass()->IsA(Scene::StaticClass()))
			m_OwningScene = (Scene*)InOwner;
		else
		{
			//Considering..
		}
	}
}
