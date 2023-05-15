#include "EnginePCH.h"
#include "SceneViewport.h"
#include "Input/Input.h"

SceneViewport::SceneViewport()
{

}

SceneViewport::~SceneViewport()
{

}

bool SceneViewport::Initialize(class Scene* InScene)
{
	m_OwningScene = InScene;

	m_cahcedInput = g_Engine->Get<Input>();

	m_CachedManager = g_Engine->Get<UIManager>();

	
	return true;
}

void SceneViewport::BeginPlay()
{
	size_t widgetCnt = m_vecWidget.size();

	for (size_t i = 0; i < widgetCnt; i++)
	{
		m_vecWidget[i]->BeginPlay();
	}
}

void SceneViewport::Update(float DeltaSeconds) 
{
	size_t widgetCnt = m_vecWidget.size();

	for (size_t i = 0; i < widgetCnt; i++)
	{
		m_vecWidget[i]->Update(DeltaSeconds);
		m_vecWidget[i]->CollisionMouse(m_cahcedInput);
	}
}

void SceneViewport::PostUpdate(float DeltaSeconds)
{
	size_t widgetCnt = m_vecWidget.size();


	for (size_t i = 0; i < widgetCnt; i++)
	{
		m_vecWidget[i]->PostUpdate(DeltaSeconds);	

	}
}

void SceneViewport::PreUpdate(float DeltaSeconds)
{
	size_t widgetCnt = m_vecWidget.size();

	for (size_t i = 0; i < widgetCnt; i++)
	{
		m_vecWidget[i]->PreUpdate(DeltaSeconds);
	}
}

void SceneViewport::Render(class Renderer* pRenderer)
{	

	size_t widgetCnt = m_vecWidget.size();

	for(size_t i=0;i<widgetCnt;i++)
	{
		m_vecWidget[i]->Render(pRenderer);
	}
	m_CachedManager->BeginUI();
	m_CachedManager->RenderUI();
	m_CachedManager->EndUI();
}

void SceneViewport::EndPlay()
{

}

bool SceneViewport::SortViewprotWidget(TSharedPtr<class Widget> Src, TSharedPtr<class Widget> Dest)
{
	return true;
	//return Src->GetZOrder() > Dest->GetZOrder();
}

bool SceneViewport::SortViewprotWidgetInv(TSharedPtr<class Widget> Src, TSharedPtr<class Widget> Dest)
{
	return true;
	//return Src->GetZOrder() < Dest->GetZOrder();
}
