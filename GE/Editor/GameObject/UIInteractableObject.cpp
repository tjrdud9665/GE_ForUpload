#include "PCH.h"
#include "UIInteractableObject.h"
#include "Assets/Sound/Sound.h"
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
#include <SceneViewport.h>
#include "Assets/Widget/UserWidget.h"

UIInteractableObject::UIInteractableObject()
	: m_UI(nullptr)
	, m_Interaction(false)
	, m_ShinyReverse(false)
	, m_Shiny(false)
	, m_ShinyValue(0.f)
	, m_ShinyTime(0.f)
{

}

UIInteractableObject::UIInteractableObject(const UIInteractableObject& Other)
	: Super(Other)
	, m_Interaction(Other.m_Interaction)
	, m_ShinyReverse(Other.m_ShinyReverse)
	, m_ShinyValue(Other.m_ShinyValue)
	, m_ShinyTime(Other.m_ShinyTime)
	, m_Shiny(Other.m_Shiny)
	, m_UIWidget(Other.m_UIWidget)
{
	m_StaticMeshCom = FindComponent<StaticMeshComponent>("UIInteractable");
}

UIInteractableObject::~UIInteractableObject()
{

}

void UIInteractableObject::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_StaticMeshCom = CreateComponent<StaticMeshComponent>("UIInteractable");

	GetRootComponent()->AddChild(m_StaticMeshCom);

}

void UIInteractableObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (m_Interaction && m_Shiny)
	{
		m_ShinyTime += DeltaSeconds;

		if (m_ShinyTime > 0.02f)
		{
			Shiny();
		}
	}
}

void UIInteractableObject::BeginPlay()
{
	Super::BeginPlay();
}

void UIInteractableObject::OnBeginInteractable(class IInteractableInterface* InteractionObject)
{
	m_Interaction = true;
}

void UIInteractableObject::OnEndInteractable()
{
	m_Interaction = false;
	m_ShinyReverse = false;
	m_ShinyTime = 0.f;
	m_ShinyValue = 0.f;

	m_StaticMeshCom->GetMaterial()->SetMetalness(0.f);
}

void UIInteractableObject::OnInteraction()
{
	if (!m_UIWidget.CustomCDOName.empty() && !m_UI)
		m_UI = g_Engine->Get<SceneViewport>()->CreateUserWidget<UserWidget>(m_UIWidget);

	if (m_UI && !m_UI->IsVisible())
		m_UI->SetVisible(true);
}

void UIInteractableObject::Shiny()
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
