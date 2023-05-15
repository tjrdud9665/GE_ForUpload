#include "PCH.h"
#include "GuizmoWidget.h"

#include "CameraManager.h"
#include "GameFramework/Components/CameraComponent.h"
#include "SceneDetails.h"
#include "../EditorManager.h"
#include "../SceneEditor.h"
#include "imgui_internal.h"



GuizmoWidget::GuizmoWidget()
	:m_AxisEditMode(EAxisEditMode::Translate)
	, m_GuizmoEditing(false)
	, m_TargetDistance(5.0f)
{

}

GuizmoWidget::~GuizmoWidget()
{

}

void GuizmoWidget::Update(float DeltaSeconds)
{	
	m_GuizmoEditing = false;

	

	ImGuizmo::SetDrawlist();
	CameraComponent* cameraComp = g_Engine->Get<CameraManager>()->GetCurrentCamera();
	Matrix matProj = cameraComp->GetProjMatrix();
	Matrix matView = cameraComp->GetViewMatrix();		
	
	ImVec2 contentSize = ImGui::GetContentRegionAvail();

	float posX = ImGui::GetWindowPos().x + m_WindowPos.x;
	float posY = ImGui::GetWindowPos().y + m_WindowPos.y;
	ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
	if (dockNode)
	{
		posX = dockNode->Pos.x;
		posY = dockNode->Pos.y;

	}
	
	ImGuizmo::SetRect(posX, posY + ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight(),
					  m_VPSize.x, m_VPSize.y);

	ImVec2 size = ImGui::GetContentRegionAvail();
	ImVec2 clipSize = ImVec2(size.x+ posX, m_VPSize.y + posY + ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight());
	ImGui::GetWindowDrawList()->PushClipRect(ImVec2(posX, posY), clipSize);
	//ImGuizmo::DrawGrid(matView.Data(), matProj.Data(), Matrix::StaticIdentity().Data(), 100.0f);


	//뷰는 카메라의월드의 역행렬		

	if (m_GuizmoTarget)
	{
		Matrix matrix = m_GuizmoTarget->GetComponentTransform().ToMatrixWithScale();
		ImGuizmo::Manipulate(matView.Data(), matProj.Data(), static_cast<ImGuizmo::OPERATION>(m_AxisEditMode), ImGuizmo::LOCAL, matrix.Data(), nullptr, nullptr);
		if (ImGuizmo::IsUsing() && ImGui::IsWindowHovered())
		{								
			m_GuizmoTarget->SetTransformFromMatrix(matrix);	
			m_GuizmoEditing = true;
		}

		HandleAxisEditModeInput();		
	}

}

void GuizmoWidget::SetViewportSize(const SVector2& InVPSize)
{
	m_VPSize.x = InVPSize.x;
	m_VPSize.y = InVPSize.y;
}

void GuizmoWidget::SetWindowPos(const SVector2& InWindowPos)
{
	m_WindowPos.x = InWindowPos.x;
	m_WindowPos.y = InWindowPos.y;
}

void GuizmoWidget::SetGuizmoTarget(class SceneComponent* InGuizmoTarget)
{
	m_GuizmoTarget = InGuizmoTarget;
}

class SceneComponent* GuizmoWidget::GetGuizmoTarget() const
{
	return m_GuizmoTarget;
}

void GuizmoWidget::SetSocketGuizmoTarget(class SkeletalMeshSocket* InMeshSocket)
{
	m_SocketTarget = InMeshSocket;
}

void GuizmoWidget::HandleAxisEditModeInput()
{	
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

	if (!is_window_hovered)
		return;



	if(!ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		if (ImGui::IsKeyDown('W'))
		{
			m_AxisEditMode = EAxisEditMode::Translate;
		}
		if (ImGui::IsKeyDown('E'))
		{
			m_AxisEditMode = EAxisEditMode::Rotate;
		}

		if (ImGui::IsKeyDown('R'))
		{
			m_AxisEditMode = EAxisEditMode::Scale;
		}

		//if (ImGui::IsKeyDown('F'))
		//{
		//	CameraManager* camMgr = g_Engine->Get<CameraManager>();

		//	CameraComponent* cameraComp = camMgr->GetCurrentCamera();

		//	if(cameraComp->GetOwner() == camMgr)
		//	{
		//		SVector3 targetLoc = SVector3::ZeroVector;
		//		if (m_GuizmoTarget)
		//		{
		//			targetLoc = m_GuizmoTarget->GetWorldLocation();
		//		}
		//		SVector3 rotator = SMathUtils::FindLookAtRotation(cameraComp->GetWorldLocation(), targetLoc);
		//		cameraComp->SetWorldRotation(rotator);	
		//	}	
		//}
	}

	//m_TargetDistance = max(1.0f,ImGui::GetIO().MouseWheel  + m_TargetDistance);	

	
}
