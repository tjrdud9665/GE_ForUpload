#include "PCH.h"
#include "SceneEditor.h"
#include "EditorWidget/SceneViewportWidget.h"
#include "EditorWidget/SceneOutliner.h"
#include "EditorWidget/SceneDetails.h"
#include "EditorWidget/GuizmoWidget.h"
#include "EditorWidget/ObjectPlacement.h"
#include "EditorWidget/NavTool.h"
#include <GameFramework/Components/CameraComponent.h>
#include <Input/Input.h>
#include <CameraManager.h>
#include <Renderer/Renderer.h>
#include "EditorWidget/SceneInfoSetting.h"
#include <SceneManager.h>

SceneEditor::SceneEditor()
{
	m_SceneViewport = nullptr;
}

SceneEditor::SceneEditor(const SceneEditor& Other)
{
	//Do Not Copy Editor.
	assert(true);
}

SceneEditor::~SceneEditor()
{
	if(g_Engine)
	{
		SceneManager* scnMgr = g_Engine->Get<SceneManager>();
		if (scnMgr)
			scnMgr->ClearBinds();
	}
	
}

void SceneEditor::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_SceneViewport = EditorWidget::CreateStandalone<SceneViewportWidget>("SceneViewport", GetSelf());
	m_SceneViewport->SetOpen(true);

	m_ObjectPlacement = EditorWidget::CreateStandalone <ObjectPlacement> ("ObjectPlacement", GetSelf());
	m_ObjectPlacement->SetOpen(true);

	m_SceneOutliner = EditorWidget::CreateStandalone<SceneOutliner>("SceneOutliner", GetSelf());
	m_SceneOutliner->SetOpen(true);

	m_SceneDetails = EditorWidget::CreateStandalone<SceneDetails>("SceneDetails", GetSelf());
	m_SceneDetails->SetOpen(true);	


	m_SceneInfoSettings = EditorWidget::CreateStandalone<SceneInfoSetting>("SceneInfoSettings", GetSelf());
	m_SceneInfoSettings->SetOpen(true);


	m_NavToolWindow = EditorWidget::CreateStandalone<NavTool>("NavTool", GetSelf());
	m_NavToolWindow->SetOpen(true);

	g_Engine->Get<SceneManager>()->BindSceneChanged(std::bind(&SceneEditor::OnSceneChanged, this, std::placeholders::_1));
	
}

void SceneEditor::ExpandViewsMenu()
{
	if (ImGui::MenuItem("SceneViewport", (const char*)0, m_SceneViewport->IsOpenedPtr())) {}

	if (ImGui::MenuItem("SceneOutliner", (const char*)0, m_SceneOutliner->IsOpenedPtr())) {}

	if (ImGui::MenuItem("SceneDetails", (const char*)0, m_SceneDetails->IsOpenedPtr())) {}

	if (ImGui::MenuItem("ObjectPlacement", (const char*)0, m_ObjectPlacement->IsOpenedPtr())) {}

	if (ImGui::MenuItem("SceneInfoSettings", (const char*)0, m_SceneInfoSettings->IsOpenedPtr())) {}

	if (ImGui::MenuItem("NavTool", (const char*)0, m_NavToolWindow->IsOpenedPtr())) {}
}

void SceneEditor::Update(float DeltaSeconds)
{
	m_SceneViewport->OnUpdate(DeltaSeconds);
		
	m_SceneOutliner->OnUpdate(DeltaSeconds);

	m_SceneDetails->OnUpdate(DeltaSeconds);

	m_ObjectPlacement->OnUpdate(DeltaSeconds);

	m_NavToolWindow->OnUpdate(DeltaSeconds);

	m_SceneInfoSettings->OnUpdate(DeltaSeconds);

}

void SceneEditor::OnSceneChanged(class Scene* changedScene)
{	
	m_SceneInfoSettings->SetCurrentScene(changedScene);
	m_SceneDetails->SetDetailTarget(nullptr);
}


