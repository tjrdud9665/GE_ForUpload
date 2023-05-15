#include "PCH.h"
#include "SceneViewportWidget.h"
#include "TargetViewer.h"
#include "Renderer/Renderer.h"
#include "RenderManager.h"
#include "D3D/D3DManager.h"
#include "GuizmoWidget.h"
#include "SceneDetails.h"
#include "../SceneEditor.h"
#include <CameraManager.h>
#include <GameFramework/Components/CameraComponent.h>
#include <GameFramework/Components/PrimitiveComponent.h>
#include "imgui_internal.h"
#include "SceneOutliner.h"
#include <SceneManager.h>
#include <Assets/Scene/Scene.h>


SceneViewportWidget::SceneViewportWidget()
{	
	m_cachedD3DManager = nullptr;
}

SceneViewportWidget::~SceneViewportWidget()
{

}

void SceneViewportWidget::Build()
{
	m_TargetViewer = EditorWidget::AddNew<TargetViewer>("SceneView");		
	m_TargetViewer->SetTargetTexID(g_Engine->Get<RenderManager>()->GetFinalTargetTextureID());

	m_Guizmo = EditorWidget::AddNew<GuizmoWidget>("Guizmo");
	m_cachedD3DManager = g_Engine->Get<D3DManager>();

	AddFlag(ImGuiWindowFlags_NoScrollbar);
	AddFlag(ImGuiWindowFlags_NoTitleBar);
	AddFlag(ImGuiWindowFlags_NoScrollWithMouse);		
	
}

void SceneViewportWidget::Update(float DeltaSeconds)
{
	if (m_Open)
	{		
		m_Guizmo->SetViewportSize(m_TargetViewer->GetTargetTexSize());		
		
		SceneDetails* details = GetOwner<SceneEditor>()->GetSceneDetails();
		if(details)
		{
			m_Guizmo->SetGuizmoTarget(details->GetGuizmoTarget());			
		}			


		ImGui::SetNextWindowSize(ImVec2((float)m_cachedD3DManager->WindowWidth(), (float)m_cachedD3DManager->WindowHeight()));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));		

		BeginWindow();
		UpdateChilds(DeltaSeconds);		

		HandleInput();
		//For EditMode
		AdjustViewport();

		MousePickComponent();
	
		
		EndWindow();

		ImGui::PopStyleVar(1);
	}
}

void SceneViewportWidget::HandleInput()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

	if (!is_window_hovered)
		return;

	if (ImGui::IsKeyDown('F'))
	{
		CameraManager* camMgr = g_Engine->Get<CameraManager>();
		CameraComponent* camComp = camMgr->GetCurrentCamera();

		
		if (camComp->GetOwner() == camMgr)
		{
			SVector3 targetLoc = SVector3::ZeroVector;
			SceneComponent* guizmoTarget = m_Guizmo->GetGuizmoTarget();
			if (guizmoTarget)
			{				
				targetLoc = guizmoTarget->GetWorldLocation();
			}
			m_TargetViewer->SetCameraFocusLocation(targetLoc);
			m_TargetViewer->SetCameraFocusMode(true);
		}
	}
	else if(ImGui::IsKeyReleased('F'))
	{
		m_TargetViewer->SetCameraFocusMode(false);
	}
}

void SceneViewportWidget::MousePickComponent()
{
	ImVec2 winSize = ImGui::GetWindowSize();
	ImVec2 winPos = ImGui::GetWindowPos();
	
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_window_hovered)
	{
		m_EnablePick = true;
	} 

	if (!is_window_hovered)
		m_EnablePick = false;

	CameraManager* camMgr = g_Engine->Get<CameraManager>();
	CameraComponent* camComp = camMgr->GetCurrentCamera();

	//카메라의 오너가 카메라메니저가 아닌 경우 Play 쪽에서 카메라를 사용중임으로 피킹을 블럭한다.
	if (camComp->GetOwner() != camMgr)
		return;


	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left,true) && is_window_hovered && !m_Guizmo->IsGuizmoEditing() && m_EnablePick)
	{
		
		SVector3 origin;
		SVector3 direction;

		D3D11_VIEWPORT viewport;//g_Engine->Get<Renderer>()->GetCurrentViewport();

		viewport.Height = winSize.y;//ImGui::GetWindowViewport()->Size.y;
		viewport.Width = winSize.x;//winSize.x;//ImGui::GetWindowViewport()->Size.x;	
		viewport.TopLeftX = winPos.x;
		viewport.TopLeftY = winPos.y;
		
		ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();		
		if (dockNode)
		{			
			viewport.Width = dockNode->Size.x;
			viewport.Height = dockNode->Size.y;
			viewport.TopLeftX = dockNode->Pos.x;
			viewport.TopLeftY = -dockNode->Pos.y;	
			
		}

		ImVec2 mouse = ImGui::GetMousePos();


		SHitResult hit = camComp->pickRay(viewport, mouse.x, mouse.y, OUT origin, OUT direction);
		if (hit.Hit)
		{
			SceneDetails* details = GetOwner<SceneEditor>()->GetSceneDetails();
			SceneOutliner* outliner = GetOwner<SceneEditor>()->GetSceneOutliner();
			details->SetDetailTarget(hit.HitComponent);
			outliner->SetPickedComponentObject((GameObject*)hit.HitComponent->GetOwner());			
		}
		else
		{
			//m_lineCmd.push_back(SDrawLineCommand(origin, origin + direction*1000.f, SLinearColor::Green));
		}
	}	
}

void SceneViewportWidget::AdjustViewport()
{

	D3D11_VIEWPORT viewport;//g_Engine->Get<Renderer>()->GetCurrentViewport();

	ImVec2 winSize = ImGui::GetWindowSize();
	ImVec2 winPos = ImGui::GetWindowPos();

	viewport.Height = winSize.y;//ImGui::GetWindowViewport()->Size.y;
	viewport.Width = winSize.x;//winSize.x;//ImGui::GetWindowViewport()->Size.x;	
	viewport.TopLeftX = winPos.x;
	viewport.TopLeftY = winPos.y;

	ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
	if (dockNode)
	{
		viewport.Width = dockNode->Size.x;
		viewport.Height = dockNode->Size.y;
		viewport.TopLeftX = dockNode->Pos.x;
		viewport.TopLeftY = -dockNode->Pos.y;
	}
	g_Engine->SetEditViewport(viewport);
}

void SceneViewportWidget::SetGuizmoTarget(class SceneComponent* InGuizmoTarget)
{
	m_Guizmo->SetGuizmoTarget(InGuizmoTarget);
}

bool SceneViewportWidget::IsGuizmoActivated() const
{
	return m_Guizmo->IsGuizmoEditing();
}

const SVector2& SceneViewportWidget::GetSize()
{
	return m_TargetViewer->GetTargetTexSize();
}
