#include "PCH.h"
#include "MeshEditor.h"
#include "Assets/Mesh/MeshAsset.h"
#include <Renderer/Renderer.h>
#include "TargetViewer.h"
#include "CameraManager.h"
#include <D3D/D3DManager.h>
#include "GameFramework/Components/CameraComponent.h"
#include "GameFramework/Components/DirectionalLightComponent.h"
#include "GameFramework/Components/StaticMeshComponent.h"
#include <Assets/Mesh/Skeleton.h>
#include <Assets/Mesh/SkeletalMeshAsset.h>
#include <Assets/Mesh/SkeletalMeshSocket.h>
#include "ImGuizmo.h"
#include "imgui_internal.h"
#include "GameFramework/Components/SkeletalMeshComponent.h"
#include <RenderManager.h>
#include "GuizmoWidget.h"
#include "imgui_stl.h"
#include "PropertySet.h"
#include <Assets/Material/Material.h>
#include <AssetManager.h>
#include <GameFramework/Components/SkyComponent.h>




MeshEditor::MeshEditor()
	:m_SelectedBoneIndex(INDEX_NONE)
	, m_HoveredBoneIndex(INDEX_NONE)
	, m_ShowAddSocketPopUp(false)
	, m_listStart(0.0f)
	, m_LeftDetailTabRatio(0.25f)
{

}

MeshEditor::MeshEditor(const MeshEditor& Other)
	:Super(Other)	
	, m_SelectedBoneIndex(INDEX_NONE)
	, m_HoveredBoneIndex(INDEX_NONE)
	, m_ShowAddSocketPopUp(false)
	, m_listStart(0.0f)
	, m_LeftDetailTabRatio(0.25f)
{
	
}

MeshEditor::~MeshEditor()
{
	SAFE_RELEASE(m_ImitateCameraManager);
	SAFE_RELEASE(m_ImitateRenderManager);		

	if(m_PreviewComponent)
	{
		m_PreviewComponent->SetMeshAsset(nullptr);
	}
}

void MeshEditor::Build()
{
	
	m_Viewer = EditorWidget::AddNew<TargetViewer>("MeshPreviewScene");	
	m_Viewer->SetDesiredSizeRatioWidth(0.8f);

	m_SocketPropertySet = EditorWidget::AddNew<PropertySet>("SocketProperties");

	m_SocketGuizmo = EditorWidget::AddNew<GuizmoWidget>("SocketGuizmo");	

	m_ImitateCameraManager = Globals::CreateEngineObject<CameraManager>();
	m_ImitateCameraManager->Initialize();
	m_ImitateCameraManager->AddRef();
	
	m_ImitateRenderManager = Globals::CreateEngineObject<RenderManager>();
	m_ImitateRenderManager->Initialize();
	m_ImitateRenderManager->AddRef();

	m_Viewer->SetTargetTexID(m_ImitateRenderManager->GetFinalTargetTextureID());

	m_SocketPreview = Globals::CreateX<SceneComponent>(SceneComponent::StaticClass(), this, "PrviewSocketComponent");

	m_PreviewDirectionalLight = Globals::CreateX<DirectionalLightComponent>(DirectionalLightComponent::StaticClass(), this, "PrviewDirectionalLight");

	m_PreviewDirectionalLight->SetWorldRotation(SVector3(65, 0, 0));
	
	m_PreviewSky = Globals::CreateX<SkyComponent>(SkyComponent::StaticClass(), this, "PreviewSky");

	AddFlag(ImGuiWindowFlags_NoDocking);	
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoCollapse);

}

void MeshEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));		
		if(BeginWindow())
		{
			ImGui::PopStyleVar(1);

			if (!m_MeshAsset)
			{
				EndWindow();
				return;
			}

			ShowMenu();
			UpdateEditSub(DeltaSeconds);

			CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
			m_ImitateCameraManager->PreUpdate(DeltaSeconds);
			m_ImitateCameraManager->Update(DeltaSeconds);
			m_ImitateCameraManager->PostUpdate(DeltaSeconds);

			ShowMeshDetails();

			m_Viewer->OnUpdate(DeltaSeconds);
			m_SocketGuizmo->OnUpdate(DeltaSeconds);
			PostUpdateEditSub();
			ShowSocketProperties(DeltaSeconds);
			ComposePreview();
			TargetViewHandleInput();

			g_Engine->Set<CameraManager>(oldCameraManager);
			EndWindow();
		}
		

	}
}

void MeshEditor::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_cachedRenderer = g_Engine->Get<Renderer>();
	
}

void MeshEditor::SetTargetMesh(class MeshAsset* InTargetMesh)
{
	m_ImitateRenderManager->ClearRenderContext();
	m_MeshAsset = InTargetMesh;
	if(m_MeshAsset)
	{
		XClass* previewCompClass = StaticMeshComponent::StaticClass();
		if (m_MeshAsset->GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
		{		
			if(m_PreviewComponent)
			{
				m_PreviewComponent->DeleteChild(m_SocketPreview);				
			}
			previewCompClass = SkeletalMeshComponent::StaticClass();
		}
	

		m_PreviewComponent = Globals::CreateX<PrimitiveComponent>(previewCompClass, this, "PrviewComponent");
		m_PreviewComponent->SetMeshAsset(m_MeshAsset);
		m_PreviewComponent->AddChild(m_SocketPreview);
		m_SocketGuizmo->SetGuizmoTarget(m_SocketPreview);
	}	

	
}

void MeshEditor::Render()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->Render();
	g_Engine->Set<CameraManager>(oldCameraManager);
}

void MeshEditor::PreRender()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->PreRender();
	g_Engine->Set<CameraManager>(oldCameraManager);
}



void MeshEditor::ShowMeshDetails()
{
	ImVec2 listSize = ImGui::GetContentRegionAvail();
	m_listStart = listSize.x * m_LeftDetailTabRatio;

	if (ImGui::BeginChild("MeshDetials", ImVec2(listSize.x * m_LeftDetailTabRatio, listSize.y)))
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs;
		if (ImGui::BeginTabBar("Mesh#left_tabs_bar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Bones"))
			{
				ShowBoneViews();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Materials"))
			{
				ShowMaterialViews();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	
}

void MeshEditor::ShowBoneViews()
{
	if(m_MeshAsset)
	{
		if(m_MeshAsset->GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
		{
			SkeletalMeshAsset* skeltalMesh = (SkeletalMeshAsset*)m_MeshAsset;
			if(skeltalMesh)
			{			
				Skeleton* skeleton = skeltalMesh->GetSkeleton();			

				if(skeleton)
				{
					ImVec2 listSize = ImGui::GetContentRegionAvail();
					//m_listStart = listSize.x * 0.25f;
					if (ImGui::BeginListBox("##boneViews", listSize))
					{					
						ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;
						m_HoveredBoneIndex = INDEX_NONE;
						m_HoveredSocket = nullptr;				

						int32 rootIdx = skeleton->GetRootBoneIndex();

						if (ImGui::TreeNodeEx(skeleton->GetBoneName(rootIdx).c_str(), flag))
						{						
							if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
							{
								m_HoveredBoneIndex = rootIdx;
							}

							TreeBone(skeleton->GetRootBoneIndex(), skeleton);			

							ImGui::TreePop();
						}
						BoneViewHandleInput();
						ImGui::EndListBox();
					}


					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
					{
						m_SelectedBoneIndex = m_HoveredBoneIndex;
						ImGui::OpenPopup("##SocketPopUp");
					
						
					}

					if (ImGui::BeginPopup("##SocketPopUp"))
					{
						ImGui::Text("Socket Menu");

						if (m_SelectedSocket)
						{
							if (ImGui::Selectable("Remove Socket"))
							{
								GetSkeletalMesh()->RemoveSocket(m_SelectedSocket);
								m_SelectedSocket = nullptr;
								m_HoveredSocket = nullptr;
							}

						}
						else if (m_SelectedBoneIndex != INDEX_NONE)
						{
							if (ImGui::Selectable("Add Socket"))
							{
								m_ShowAddSocketPopUp = true;
								m_NewSocketName = "";
							}
						}
						
						ImGui::EndPopup();
					}	
				
				
				}			
			}	

			if(m_ShowAddSocketPopUp)
			{
				ImGui::OpenPopup("AddSocketPopup");
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				if (ImGui::BeginPopupModal("AddSocketPopup"))
				{				
					ImGui::InputText("##newSocketname", m_NewSocketName);
					if (ImGui::Button("AddSocket!"))
					{
						SkeletalMeshAsset* skeletalMesh = GetSkeletalMesh();
						if (skeletalMesh)
						{
							if(skeletalMesh->AddSkeletalMeshSocket(skeletalMesh->GetSkeleton()->GetBoneName(m_SelectedBoneIndex), m_NewSocketName))
							{
								m_ShowAddSocketPopUp = false;
							}
						}
					}
					ImGui::SameLine();
					if(ImGui::Button("Close"))
					{
						m_ShowAddSocketPopUp = false;
					}
					ImGui::EndPopup();
				}
			}
			

		}

	}

	ImGui::SameLine();	
}


void MeshEditor::ShowMaterialViews()
{
	if (m_MeshAsset)
	{
		std::vector<Material*>	matSlots;
		size_t slotCnt = m_MeshAsset->GetMaterials_NotCloned(OUT matSlots);
		std::vector<AssetData*> MaterialAssets;		
		size_t materialAssetSize = g_Engine->Get<AssetManager>()->GetAssetsFromInheritedType(OUT MaterialAssets, Material::StaticClass());
		
		bool bDirty = false;
		
		for(size_t i=0;i<slotCnt;i++)
		{
			bool exsist = false;
			std::string RelPathWithName = "NONE";
			if (matSlots[i])
			{
				matSlots[i]->GetRelativePathWithName(OUT RelPathWithName);
				exsist = true;

			}
			ImGui::Text("SlotName : ");
			ImGui::SameLine();
			ImGui::Text(m_MeshAsset->GetMesh((int32)i).GetMeshName().c_str());

			std::stringstream ss;
			ss << "##mat_slot_" << i;						
			if(ImGui::BeginCombo(ss.str().c_str(),RelPathWithName.c_str(), ImGuiComboFlags_None))
			{
				//show Material Selections
				for (int32 j = 0; j < materialAssetSize; j++)
				{
					bool is_selected = MaterialAssets[j] == matSlots[i];
					std::string SelectedRelPathWithName;
					MaterialAssets[j]->GetRelativePathWithName(OUT SelectedRelPathWithName);
					if (ImGui::Selectable(SelectedRelPathWithName.c_str(), is_selected))
					{
						matSlots[i] = (Material*)MaterialAssets[j];
						bDirty = true;
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::Text("Material");
						ImGui::Text(SelectedRelPathWithName.c_str());					

						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				

				ImGui::EndCombo();
			}
			if (bDirty)
			{
				m_MeshAsset->SetMaterials(matSlots);
				m_PreviewComponent->SetMeshAsset(m_MeshAsset);
			}
			
		}
		

	}
}

void MeshEditor::ShowSocketProperties(float DeltaSeconds)
{
	ImGui::SameLine();

	float cursor = ImGui::GetContentRegionAvail().x;
	
	ImGui::SetCursorPosX(cursor + m_listStart);
	ImGui::BeginChild("socket");
	m_SocketPropertySet->OnUpdate(DeltaSeconds);
	ImGui::EndChild();
	
	if (m_SelectedSocket)
	{
		m_SocketPreview->SetRelativeLocation(m_SelectedSocket->GetRelativeLocation());
		m_SocketPreview->SetRelativeRotation(m_SelectedSocket->GetRelativeRotation());
		m_SocketPreview->SetRelativeScale(m_SelectedSocket->GetRelativeScale());
	}

}

void MeshEditor::TreeBone(int32 parentIdx, class Skeleton* InSkeleton)
{
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding  | ImGuiTreeNodeFlags_DefaultOpen  | ImGuiTreeNodeFlags_SpanFullWidth;

	std::vector<int32> childIdx;
	size_t childCount = InSkeleton->GetChilds(parentIdx,OUT childIdx);	
	

	for (size_t i = 0; i < childCount; i++)
	{
		if (m_SelectedBoneIndex == childIdx[i])
			flag |= ImGuiTreeNodeFlags_Selected;
		else		
			flag &= ~ImGuiTreeNodeFlags_Selected;

		std::vector<int32> child_childIdx;

		if(InSkeleton->GetChilds(childIdx[i],OUT child_childIdx) == 0)
			flag |= ImGuiTreeNodeFlags_Leaf;
		
		std::vector<class SkeletalMeshSocket*> childSockets;
		size_t childSocketSize = GetSkeletalMesh()->GetSkeletalMeshSocketFromBoneName(InSkeleton->GetBoneName(childIdx[i]),OUT childSockets);

		if (ImGui::TreeNodeEx(InSkeleton->GetBoneName(childIdx[i]).c_str(), flag))
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
			{
				m_HoveredBoneIndex = childIdx[i];
			}
			
			TreeBone(childIdx[i], InSkeleton);

			for(size_t s =0;s<childSocketSize;s++)
			{
				if(childSockets[s] == m_SelectedSocket)
					flag |= ImGuiTreeNodeFlags_Selected;
				else
					flag &= ~ImGuiTreeNodeFlags_Selected;

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
				if (ImGui::TreeNodeEx(childSockets[s]->GetSocketName().c_str(), flag))
				{
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
					{
						m_HoveredSocket = childSockets[s];
					}
					ImGui::TreePop();
				}
				ImGui::PopStyleColor();
			}
		
			ImGui::TreePop();
		}
	}
	
	
}

void MeshEditor::BoneViewHandleInput()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	const bool left_click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	const bool right_click = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	if (!is_window_hovered)
		return;


	if (left_click)
	{		
		m_SelectedBoneIndex = m_HoveredBoneIndex;
		m_SelectedSocket = m_HoveredSocket;

		m_SocketPropertySet->SetPropertyTarget(m_SelectedSocket);
		if (m_SelectedSocket)
		{			
			m_SocketPreview->SetRelativeLocation(m_SelectedSocket->GetRelativeLocation());
			m_SocketPreview->SetRelativeRotation(m_SelectedSocket->GetRelativeRotation());
			m_SocketPreview->SetRelativeScale(m_SelectedSocket->GetRelativeScale());		
			m_SocketPreview->SetAttachedSocketName(m_SelectedSocket->GetSocketName());
		}
		else if (m_SelectedBoneIndex != INDEX_NONE)
		{			
			m_SocketPreview->SetAttachedSocketName(GetSkeletalMesh()->GetSkeleton()->GetBoneName(m_SelectedBoneIndex));
		}
	}

}

void MeshEditor::ComposePreview()
{
	if (m_PreviewComponent)
		m_ImitateRenderManager->AddRenderList(m_PreviewComponent);

	m_ImitateRenderManager->AddRenderList(m_PreviewDirectionalLight);

	m_ImitateRenderManager->AddRenderList(m_PreviewSky);
	
}

void MeshEditor::UpdateEditSub(float DeltaSeconds)
{
	ImVec2 contentSize = ImGui::GetContentRegionAvail();
	m_SocketGuizmo->SetWindowPos(SVector2(contentSize.x * m_LeftDetailTabRatio, 0.0f));
	m_SocketGuizmo->SetViewportSize(m_Viewer->GetTargetTexSize());

	m_PreviewComponent->PreUpdate(DeltaSeconds);
	m_PreviewComponent->Update(DeltaSeconds);
	m_PreviewComponent->PostUpdate(DeltaSeconds);

	if (!m_SelectedSocket)
	{
		m_SocketPreview->SetRelativeLocation(SVector3::ZeroVector);
		m_SocketPreview->SetRelativeRotation(SVector3::ZeroVector);
		m_SocketPreview->SetRelativeScale(SVector3::OneVector);
	}
}

void MeshEditor::PostUpdateEditSub()
{
	if(m_SelectedSocket)
	{
		m_SelectedSocket->SetRelativeLocation(m_SocketPreview->GetRelativeLocation());
		m_SelectedSocket->SetRelativeRotation(m_SocketPreview->GetRelativeRotation());
		m_SelectedSocket->SetRelativeScale(m_SocketPreview->GetRelativeScale3D());
	}
}

void MeshEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_MeshAsset)
					m_MeshAsset->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	
}

void MeshEditor::TargetViewHandleInput()
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
			SceneComponent* guizmoTarget = m_SocketGuizmo->GetGuizmoTarget();
			if (guizmoTarget)
			{
				targetLoc = guizmoTarget->GetWorldLocation();
			}
			m_Viewer->SetCameraFocusLocation(targetLoc);
			m_Viewer->SetCameraFocusMode(true);
		}
	} else if (ImGui::IsKeyReleased('F'))
	{
		m_Viewer->SetCameraFocusMode(false);
	}
}

class SkeletalMeshAsset* MeshEditor::GetSkeletalMesh()
{
	if (m_MeshAsset)
	{
		if (m_MeshAsset->GetClass()->IsA(SkeletalMeshAsset::StaticClass()))
		{
			return (SkeletalMeshAsset*)m_MeshAsset;
		}
	}

	return nullptr;
}

