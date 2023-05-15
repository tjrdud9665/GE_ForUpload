#include "PCH.h"
#include "GameObjectEditor.h"
#include "TargetViewer.h"
#include <GameFramework/Components/SkyComponent.h>
#include <CameraManager.h>
#include <RenderManager.h>
#include "GameFramework/GameObject/GameObject.h"
#include "ComponentDetails.h"
#include "GameFramework/Components/CameraComponent.h"
#include "GameFramework/Components/DirectionalLightComponent.h"
#include "GuizmoWidget.h"
#include "PropertySet.h"
#include "SceneDetailWidget.h"
#include <Physics/PhysicsSystem.h>
#include <Physics/PhysicsDebugDraw.h>



GameObjectEditor::GameObjectEditor()
	:m_ObjectEditTarget(INDEX_NONE)
	, m_EditObject(nullptr)
	, m_Viewer(nullptr)
	, m_ImitateCameraManager(nullptr)
	, m_ImitateRenderManager(nullptr)
	, m_ObjectEditGuizmo(nullptr)
	, m_viewportSizeRatio(0.8f)
{

}

GameObjectEditor::GameObjectEditor(const GameObjectEditor& Other)
	:Super(Other)
	, m_viewportSizeRatio(0.8f)
{

}

GameObjectEditor::~GameObjectEditor()
{
	SAFE_RELEASE(m_ImitateCameraManager);
	SAFE_RELEASE(m_ImitateRenderManager);


}

void GameObjectEditor::Build()
{
	m_Viewer = EditorWidget::AddNew<TargetViewer>("GameObjectPreviewScene");
	m_Viewer->SetDesiredSizeRatioWidth(m_viewportSizeRatio);

	m_ObjectEditGuizmo = EditorWidget::AddNew<GuizmoWidget>("ObjectEditGuizmo");
	m_ObjectDetails = EditorWidget::AddNew<SceneDetailWidget>("ObjectEditDetail");
	

	m_ImitateCameraManager = Globals::CreateEngineObject<CameraManager>();
	m_ImitateCameraManager->Initialize();
	m_ImitateCameraManager->AddRef();

	m_ImitateRenderManager = Globals::CreateEngineObject<RenderManager>();
	m_ImitateRenderManager->Initialize();
	m_ImitateRenderManager->AddRef();
	

	m_PreviewDirectionalLight = Globals::CreateX<DirectionalLightComponent>(DirectionalLightComponent::StaticClass(), this, "PrviewDirectionalLight");

	m_PreviewSky = Globals::CreateX<SkyComponent>(SkyComponent::StaticClass(), this, "PreviewSky");

	m_PreviewDirectionalLight->SetWorldRotation(SVector3(65, 0, 0));

	m_Viewer->SetTargetTexID(m_ImitateRenderManager->GetFinalTargetTextureID());
	
	AddFlag(ImGuiWindowFlags_NoDocking);
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoCollapse);

}

void GameObjectEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (BeginWindow())
		{
			ImGui::PopStyleVar(1);

			if (!m_EditObject)
			{
				EndWindow();
				return;
			}

			ShowMenu();
			UpdateEditSub(DeltaSeconds);

			CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
			PhysicsSystem* physics = g_Engine->Get<PhysicsSystem>();
			RenderManager* prevRender = physics->GetDebugDraw()->SetImitateRenderManager(m_ImitateRenderManager);


			m_ImitateCameraManager->PreUpdate(DeltaSeconds);
			m_ImitateCameraManager->Update(DeltaSeconds);
			m_ImitateCameraManager->PostUpdate(DeltaSeconds);
			

			m_Viewer->OnUpdate(DeltaSeconds);			
			m_ObjectEditGuizmo->OnUpdate(DeltaSeconds);		
			ShowObjectDetail(DeltaSeconds);

			
			//UpdateEditorWorld
			physics->DrawEditorWorld();

			ComposePreview();
			HandleInput();
			g_Engine->Set<CameraManager>(oldCameraManager);
			physics->GetDebugDraw()->SetImitateRenderManager(prevRender);
			EndWindow();

		}

	}
}

void GameObjectEditor::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_cachedRenderer = g_Engine->Get<Renderer>();
}

void GameObjectEditor::Render()
{
	PhysicsSystem* physics = g_Engine->Get<PhysicsSystem>();
	RenderManager* prevRender =  physics->GetDebugDraw()->SetImitateRenderManager(m_ImitateRenderManager);

	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->Render();
	g_Engine->Set<CameraManager>(oldCameraManager);

	physics->GetDebugDraw()->SetImitateRenderManager(prevRender);
	
}

void GameObjectEditor::PreRender()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->PreRender();
	g_Engine->Set<CameraManager>(oldCameraManager);
}

void GameObjectEditor::SetTargetObject(class GameObject* InObject)
{
	PhysicsSystem* physics = g_Engine->Get<PhysicsSystem>();
	physics->ClearEditorWorld();		

	m_EditObject = InObject;
	
	m_ObjectDetails->SetDetailTarget(m_EditObject);
}

void GameObjectEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_EditObject)
					m_EditObject->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void GameObjectEditor::ComposePreview()
{
	if (m_EditObject)
		m_EditObject->PushRenderList(m_ImitateRenderManager);


	m_ImitateRenderManager->AddRenderList(m_PreviewDirectionalLight);
	m_ImitateRenderManager->AddRenderList(m_PreviewSky);
	
	
}

void GameObjectEditor::UpdateEditSub(float DeltaSeconds)
{
	ImVec2 contentSize = ImGui::GetContentRegionAvail();
	m_ObjectEditGuizmo->SetGuizmoTarget(m_ObjectDetails->GetGuizmoTarget());
	m_ObjectEditGuizmo->SetWindowPos(SVector2(0.0f, 0.0f));
	m_ObjectEditGuizmo->SetViewportSize(m_Viewer->GetTargetTexSize());


	RenderManager* originMgr = g_Engine->Get<RenderManager>();
	originMgr->AddRef();
	g_Engine->Set<RenderManager>(m_ImitateRenderManager);

	m_EditObject->PreUpdate(DeltaSeconds);
	m_EditObject->Update(DeltaSeconds);
	m_EditObject->PostUpdate(DeltaSeconds);

	g_Engine->Set<RenderManager>(originMgr);
	originMgr->Release();



}

void GameObjectEditor::ShowObjectDetail(float DeltaSeconds)
{
	ImGui::SameLine();

	float cursor = ImGui::GetContentRegionAvail().x;

	ImGui::SetCursorPosX(cursor* m_viewportSizeRatio);
	ImGui::BeginChild("ObjectDetail",ImVec2(0,0),false);
	m_ObjectDetails->OnUpdate(DeltaSeconds);
	

	ImGui::EndChild();


}

void GameObjectEditor::HandleInput()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

	if (!is_window_hovered)
		return;

	if (ImGui::IsKeyDown('F'))
	{
		CameraComponent* camComp = m_ImitateCameraManager->GetCurrentCamera();

		if (camComp)
		{
			SVector3 targetLoc = SVector3::ZeroVector;
			SceneComponent* guizmoTarget = m_ObjectEditGuizmo->GetGuizmoTarget();
			if (guizmoTarget)
			{
				targetLoc = guizmoTarget->GetWorldLocation();
			}
			m_Viewer->SetCameraFocusLocation(targetLoc);
			m_Viewer->SetCameraFocusMode(true);
		}
	} 
	else if (ImGui::IsKeyReleased('F'))
	{
		m_Viewer->SetCameraFocusMode(false);
	}
}
