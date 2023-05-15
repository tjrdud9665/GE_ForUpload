#include "PCH.h"
#include "TransformPropertyWidget.h"
#include "GameFramework/Components/SceneComponent.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "../EditorManager.h"
#include "../SceneEditor.h"
#include "SceneViewportWidget.h"

TransformPropertyWidget::TransformPropertyWidget()
{
	m_DrawLocationModePopup = false;
	m_DrawRotationModePopup = false;
	m_DrawScale3DModePopup = false;

}

TransformPropertyWidget::~TransformPropertyWidget()
{

}

void TransformPropertyWidget::Update(float DeltaSeconds)
{
	SceneComponent* val = nullptr;
	GetHandle()->GetValue(val);
	bool HasOwningScene = false;
	bool IsRoot = false;
		

	if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))&& !ImGui::IsAnyItemHovered())
	{
		m_DrawLocationModePopup = false;
		m_DrawRotationModePopup = false;
		m_DrawScale3DModePopup = false;
	}

	if (val)
	{
		float dragingRate = 0.1f;

		
		if (ImGui::GetMergedModFlags() == ImGuiKeyModFlags_Ctrl)
			dragingRate = 0.01f;

		std::string AbsoluteText = "Absolute";
		std::string PositionLabel = "Location";
		std::string RotationLabel = "Rotation";
		std::string ScaleLabel = "Scale";

		std::string composedPostionLabel = PositionLabel;
		std::string composedRotationLabel = RotationLabel;
		std::string composedScaleLabel = ScaleLabel;

		bool postionOptPop = false;
		bool IsAbsoultePostion = false;
		bool IsAbsoulteRotation = false;
		bool IsAbsoulteScale = false;

		if(val->GetOwningScene())
		{
			HasOwningScene = true;
		}
		if(!val->GetAttachParent())
		{
			IsRoot = true;
		}
		

		if(val->IsUsingAbsoluteLocation())
		{
			composedPostionLabel = AbsoluteText + PositionLabel;
			IsAbsoultePostion = true;
		}		
		if(val->IsUsingAbsoluteRotation())
		{
			composedRotationLabel = AbsoluteText + RotationLabel;
			IsAbsoulteRotation = true;
		}
		if(val->IsUsingAbsoluteScale())
		{
			composedScaleLabel = AbsoluteText + ScaleLabel;
			IsAbsoulteScale = true;
		}
		if (!HasOwningScene && IsRoot)
			return;

		//1.Position
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();

			ImGuiTreeNodeFlags flag =ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
			ImGui::SetNextItemOpen(m_DrawLocationModePopup);
			
			if (ImGui::TreeNodeEx(composedPostionLabel.c_str(), flag))
			{
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::IsItemClicked())
						m_DrawLocationModePopup = true;
				}
				ImGui::OpenPopup("location_mode");
			
				if (ImGui::BeginPopup("location_mode"))
				{
					ImGui::Text("Location Type");
					ImGui::Separator();
					if (ImGui::Selectable("Relative"))
					{
						m_DrawLocationModePopup = false;
						val->SetAbsoluteLocation(false);

					}
					if (ImGui::Selectable("World"))
					{
						m_DrawLocationModePopup = false;
						if(val->GetAttachParent())
						{
							STransform ParentToWorld = val->GetAttachParent()->GetSocketTransform(val->GetAttachSocketName());
							STransform test = val->m_ComponentToWorld * ParentToWorld;
							SVector3 worldLocation = test.Translation;
							val->SetWorldLocation(worldLocation);
						}						

						val->SetAbsoluteLocation(true);
					}
					ImGui::EndPopup();
				}				

				ImGui::TreePop();
			}			

			ImGui::TableSetColumnIndex(1);			
								
			if (IsAbsoultePostion)
			{	
				SVector3 worldLocation = val->GetWorldLocation();
				ImGui::DragFloat3("##position", worldLocation.Data(), dragingRate, -FLT_MAX, FLT_MAX, "%g");
				if(!g_Engine->Get<EditorManager>()->FindEditor<SceneEditor>("SceneEditor")->GetSceneViewport()->IsGuizmoActivated())
					val->SetWorldLocation(worldLocation);
				
			}
			else
			{
				ImGui::DragFloat3("##position", val->m_RelativeLocation.Data(), dragingRate, -FLT_MAX, FLT_MAX, "%g");
			}
			ImGui::EndTable();
		}//EndPostions

		//2.Rotations
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();

			ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
			ImGui::SetNextItemOpen(m_DrawRotationModePopup);

			if (ImGui::TreeNodeEx(composedRotationLabel.c_str(), flag))
			{
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::IsItemClicked())
						m_DrawRotationModePopup = true;
				}
				ImGui::OpenPopup("rotation_mode");

				if (ImGui::BeginPopup("rotation_mode"))
				{
					ImGui::Text("Rotation Type");
					ImGui::Separator();
					if (ImGui::Selectable("Relative"))
					{
						m_DrawRotationModePopup = false;
						val->SetAbsoluteRotation(false);

					}
					if (ImGui::Selectable("World"))
					{
						m_DrawRotationModePopup = false;
						val->SetAbsoluteRotation(true);
					}
					ImGui::EndPopup();
				}

				ImGui::TreePop();
			}

			ImGui::TableSetColumnIndex(1);								
			ImGui::DragFloat3("##rotation", val->m_RelativeRotation.Data(), dragingRate, -FLT_MAX, FLT_MAX, "%g");		
			ImGui::EndTable();
		}//End of Rotations


		//2.Scale3D
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();

			ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
			ImGui::SetNextItemOpen(m_DrawScale3DModePopup);

			if (ImGui::TreeNodeEx(composedScaleLabel.c_str(), flag))
			{
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::IsItemClicked())
						m_DrawScale3DModePopup = true;
				}
				ImGui::OpenPopup("scale_mode");

				if (ImGui::BeginPopup("scale_mode"))
				{
					ImGui::Text("Scale Type");
					ImGui::Separator();
					if (ImGui::Selectable("Relative"))
					{
						m_DrawScale3DModePopup = false;
						val->SetAbsoluteScale(false);

					}
					if (ImGui::Selectable("World"))
					{
						m_DrawScale3DModePopup = false;
						val->SetAbsoluteScale(true);
					}
					ImGui::EndPopup();
				}

				ImGui::TreePop();
			}

			ImGui::TableSetColumnIndex(1);

			ImGui::DragFloat3("##scale", val->m_RelativeScale3D.Data(), dragingRate, -FLT_MAX, FLT_MAX, "%g");

			ImGui::EndTable();
		}//End of Scale

	}
}

