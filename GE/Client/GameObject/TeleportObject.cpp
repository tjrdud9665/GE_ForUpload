#include "PCH.h"
#include "TeleportObject.h"
//#include "Assets/Sound/Sound.h"
#include "SceneManager.h"
#include "Assets/Scene/Scene.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "Path/PathManager.h"
#include "GameFramework/Components/StaticMeshComponent.h"
#include "GameFramework/Components/PrimitiveComponent.h"
#include "Assets/AssetData.h"
#include "Serialization/Archive.h"
#include "GameFramework/Components/SphereComponent.h"
#include "Assets/Material/Material.h"
#include "SceneViewport.h"
#include <Assets/Widget/UserWidget.h>

TeleportObject::TeleportObject()
	: m_Destionation(nullptr)
	, m_Interaction(false)
	, m_ShinyReverse(false)
	, m_ShinyValue(0.f)
	, m_ShinyTime(0.f)
	, m_Activate(false)
{

}

TeleportObject::TeleportObject(const TeleportObject& Other)
	: Super(Other)
	, m_Destionation(Other.m_Destionation)
	, m_Interaction(Other.m_Interaction)
	, m_ShinyReverse(Other.m_ShinyReverse)
	, m_ShinyValue(Other.m_ShinyValue)
	, m_ShinyTime(Other.m_ShinyTime)
	, m_Activate(false)
	, m_InteractionWidgetClass(Other.m_InteractionWidgetClass)

{
	m_StaticMeshCom = FindComponent<StaticMeshComponent>("Teleport");
}

TeleportObject::~TeleportObject()
{

}

void TeleportObject::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_StaticMeshCom = CreateComponent<StaticMeshComponent>("Teleport");

	GetRootComponent()->AddChild(m_StaticMeshCom);

}


void TeleportObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (m_Activate)
	{
		m_ShinyTime += DeltaSeconds;
		
		if (m_ShinyTime > 0.02f)
		{	
			Shiny();
		}
	}




}

void TeleportObject::BeginPlay()
{
	Super::BeginPlay();


	if (!m_InteractionWidgetClass.CustomCDOName.empty())
	{
		m_WidgetInstance = g_Engine->Get<SceneViewport>()->CreateUserWidget<UserWidget>(m_InteractionWidgetClass);
		m_WidgetInstance->SetVisible(false);
	}
}

void TeleportObject::OnBeginInteractable(class IInteractableInterface* InteractionObject)
{
	// 충돌시 이펙트 처리 
	
	m_Interaction = true;	

	if (m_Activate && m_WidgetInstance)
		m_WidgetInstance->SetVisible(true);

}

void TeleportObject::OnEndInteractable()
{
	// 충돌 해제시 원복 
	m_Interaction = false;
	if (m_Activate && m_WidgetInstance)
		m_WidgetInstance->SetVisible(false);
	//m_ShinyReverse = false;
	//m_ShinyTime = 0.f;
	//m_ShinyValue = 0.f;

	//m_StaticMeshCom->GetMaterial()->SetMetalness(0.f);
}

void TeleportObject::OnInteraction()
{
	// 플레이어에서 키입력시 함수 호출됨 
	// UI 연결 필요

	if (m_Activate && m_Interaction)
		TeleportScene();
}

void TeleportObject::ActivateTeleport()
{
	m_Activate = true;
}

void TeleportObject::TeleportScene()
{
	// UI 버튼에 이 함수 연결필요
	if (!m_Destionation)
		return;

	g_Engine->Get<SceneManager>()->CreateNextScene(m_Destionation);
	g_Engine->Get<SceneManager>()->SetAutoPlay(true);	
}

void TeleportObject::Shiny()
{
	if (m_ShinyReverse)
		--m_ShinyValue;

	else if (m_ShinyValue < 30.f)
		++m_ShinyValue;

	if (m_ShinyValue >= 30.f)
		m_ShinyReverse = true;

	else if (m_ShinyValue <= 0.f)
		m_ShinyReverse = false;

	m_StaticMeshCom->GetMaterial()->SetMetalness(m_ShinyValue);

	m_ShinyTime = 0.f;
}

