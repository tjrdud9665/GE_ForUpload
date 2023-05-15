#include "PCH.h"
#include "CameraControlWidget.h"
#include "imgui.h"
#include "imgui_stl.h"
#include "CameraManager.h"
#include "GameFramework/Components/CameraComponent.h"
#include "imgui_internal.h"

CameraControlWidget::CameraControlWidget()
	:m_CameraMoveSpeed(100.0f)
{

}

CameraControlWidget::~CameraControlWidget()
{

}

void CameraControlWidget::Build()
{
	m_FocusMode = false;
}

void CameraControlWidget::Update(float DeltaSeconds)
{			
	CameraManager* camMgr = g_Engine->Get<CameraManager>();

	bool IsEditorCamera = camMgr->GetCurrentCamera()->GetOwner() == camMgr;

		
	ImVec2 pos = ImGui::GetItemRectMin() ;
	pos.x -= ImGui::GetWindowPos().x;
	pos.y -= ImGui::GetWindowPos().y;

	ImGui::SetCursorPos(pos);
		
	if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable, ImVec2(250, 0)))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("cameLoc");
		ImGui::TableSetColumnIndex(1);
		SVector3 camLoc = camMgr->GetCurrentCamera()->GetWorldLocation();
		ImGui::DragFloat3("##camLoc", camLoc.Data(), 0.1f, -FLT_MAX, FLT_MAX, "%g");

		if(camMgr->GetCurrentCamera()->GetOwner() == camMgr)
		{
			camMgr->GetCurrentCamera()->SetWorldLocation(camLoc);
		}
		

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("cameRot");
		ImGui::TableSetColumnIndex(1);
		SVector3 camRot = camMgr->GetCurrentCamera()->GetWorldRotation();
		ImGui::DragFloat3("##camRot", camRot.Data(), 0.1f, -FLT_MAX, FLT_MAX, "%g");
		if (camMgr->GetCurrentCamera()->GetOwner() == camMgr)
		{
			camMgr->GetCurrentCamera()->SetWorldRotation(camRot);
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("speed");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat("##spd", &m_CameraMoveSpeed);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("FocusOn(F)"); ImGui::SameLine();
		ImGui::Checkbox("##FocusOn", &m_FocusMode);
		ImGui::TableSetColumnIndex(1);

		if(!m_FocusMode)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}		

		ImGui::DragFloat3("##foucsloc", m_WorldFoucsLocation.Data(), 0.1f, -FLT_MAX, FLT_MAX, "%g");

		if (!m_FocusMode)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}


		ImGui::EndTable();
	}	
	
	if (IsEditorCamera)
	{
		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			Matrix CameraMatrix = camMgr->GetCurrentCamera()->GetComponentTransform().ToMatrixNoScale();

			SVector3	desiredDirection = SVector3::ZeroVector;

			ImGuiIO& io = ImGui::GetIO();

			if (ImGui::IsKeyDown('W'))
			{
				desiredDirection += CameraMatrix.v[2];
			}
			if (ImGui::IsKeyDown('S'))
			{
				desiredDirection += -CameraMatrix.v[2];
			}
			if (ImGui::IsKeyDown('D'))
			{
				desiredDirection += CameraMatrix.v[0];
			} else if (ImGui::IsKeyDown('A'))
			{
				desiredDirection += -CameraMatrix.v[0];
			}

			desiredDirection.Normalize();

			camMgr->GetCurrentCamera()->AddWorldLocation(DeltaSeconds * m_CameraMoveSpeed * desiredDirection);

			if(!m_FocusMode)
			{
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
				{
					SVector3 Rotator = SVector3::ZeroVector;

					Rotator += io.MouseDelta.x * SVector3::UpVector * 0.5f;
					Rotator += io.MouseDelta.y * SVector3::RightVector * 0.5f;

					//SMathUtils::FindLookAtRotation()
					camMgr->GetCurrentCamera()->AddWorldRotation(Rotator);
				}
			}
		}

		if (m_FocusMode)
		{	
			SVector3 rotator = SMathUtils::FindLookAtRotation(camMgr->GetCurrentCamera()->GetWorldLocation(), m_WorldFoucsLocation);
			camMgr->GetCurrentCamera()->SetWorldRotation(rotator);
			

		}
	}
}

void CameraControlWidget::SetFocusMode(bool focusMode)
{
	m_FocusMode = focusMode;
}

void CameraControlWidget::SetFocusLocation(const SVector3& InLocation)
{
	m_WorldFoucsLocation = InLocation;
}
