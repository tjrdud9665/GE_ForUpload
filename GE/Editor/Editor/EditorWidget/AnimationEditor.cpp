#include "PCH.h"
#include "AnimationEditor.h"
#include "AnimationSequencer.h"
#include <Renderer/Renderer.h>
#include "TargetViewer.h"
#include "CameraManager.h"
#include <D3D/D3DManager.h>
#include "GameFramework/Components/CameraComponent.h"
#include "GameFramework/Components/DirectionalLightComponent.h"
#include "GameFramework/Components/StaticMeshComponent.h"
#include <Assets/Mesh/Skeleton.h>
#include <Assets/Mesh/SkeletalMeshAsset.h>
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

AnimationEditor::AnimationEditor()
{
	
	m_PropPosY = 0.0f;
}

AnimationEditor::AnimationEditor(const AnimationEditor& Other)
	:Super(Other)
{
	//Not use 
	
}

AnimationEditor::~AnimationEditor()
{
	SAFE_RELEASE(m_ImitateCameraManager);
	SAFE_RELEASE(m_ImitateRenderManager);

	if (m_AnimPreviewComponent)
	{
		m_AnimPreviewComponent->SetAnimationSequence(nullptr);
	}
}

void AnimationEditor::Build()
{
	Super::Build();

	
	m_TargetViewRatioX = 0.8f;
	m_Viewer = EditorWidget::AddNew<TargetViewer>("AnimPreviewScene");
	
	m_Viewer->SetDesiredSizeRatioWidth(m_TargetViewRatioX);

	m_ImitateCameraManager = Globals::CreateEngineObject<CameraManager>();
	m_ImitateCameraManager->Initialize();
	m_ImitateCameraManager->AddRef();

	m_ImitateRenderManager = Globals::CreateEngineObject<RenderManager>();
	m_ImitateRenderManager->Initialize();
	m_ImitateRenderManager->AddRef();

	m_Viewer->SetTargetTexID(m_ImitateRenderManager->GetFinalTargetTextureID());
	

	m_PreviewDirectionalLight = Globals::CreateX<DirectionalLightComponent>(DirectionalLightComponent::StaticClass(), this, "PrviewDirectionalLight");

	m_PreviewDirectionalLight->SetWorldRotation(SVector3(65, 0, 0));

	m_PreviewSky = Globals::CreateX<SkyComponent>(SkyComponent::StaticClass(), this, "PreviewSky");

	m_AnimPreviewComponent = Globals::CreateX<SkeletalMeshComponent>(SkeletalMeshComponent::StaticClass(), this, "Preview");


	m_Sequencer = EditorWidget::AddNew<AnimationSequencer>("AnimationSequencer");
	m_Sequencer->InitPreviewComponent(m_AnimPreviewComponent);

	m_NotifyInfoPropertySet = EditorWidget::AddNew<PropertySet>("AnimNotifyPorpertySet");	

	AddFlag(ImGuiWindowFlags_NoDocking);
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoCollapse);		

	m_cachedAssetManager = g_Engine->Get<AssetManager>();
	
}

void AnimationEditor::SetTargetAnimation(class AnimationSequence* AnimSeq)
{
	m_TargetAnimSeq = AnimSeq;
	if(m_TargetAnimSeq)
	{		

		MeshAsset* previewAsset =  (MeshAsset*)m_cachedAssetManager->FindAssetFromName(m_TargetAnimSeq->GetSkeleton()->GetModelMeshName(), SkeletalMeshAsset::StaticClass());

		m_AnimPreviewComponent->SetMeshAsset(previewAsset);
		m_AnimPreviewComponent->SetAnimationSequence(AnimSeq);
		m_Sequencer->SetTargetAnimation(m_TargetAnimSeq);
		m_NotifyInfoPropertySet->SetPropertyTarget(m_TargetAnimSeq);
	}
}

void AnimationEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (BeginWindow())
		{
			ImGui::PopStyleVar(1);

			if (!m_TargetAnimSeq)
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

			m_PropPosY = ImGui::GetCursorPos().y;
			m_PropPosX = ImGui::GetCursorPos().x;
			m_Viewer->OnUpdate(DeltaSeconds);			
			
			ShowAnimationSequencer(DeltaSeconds);
			ComposePreview();
			TargetViewHandleInput();
			g_Engine->Set<CameraManager>(oldCameraManager);
			EndWindow();
		}


	}
}

void AnimationEditor::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
}

void AnimationEditor::Render()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->Render();
	g_Engine->Set<CameraManager>(oldCameraManager);
}

void AnimationEditor::PreRender()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->PreRender();
	g_Engine->Set<CameraManager>(oldCameraManager);
}

void AnimationEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_TargetAnimSeq)
					m_TargetAnimSeq->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void AnimationEditor::UpdateEditSub(float DeltaSeconds)
{
	m_AnimPreviewComponent->PreUpdate(DeltaSeconds);
	m_AnimPreviewComponent->Update(DeltaSeconds);
	m_AnimPreviewComponent->PostUpdate(DeltaSeconds);
}

void AnimationEditor::ComposePreview()
{
	if (m_AnimPreviewComponent)
		m_ImitateRenderManager->AddRenderList(m_AnimPreviewComponent);

	m_ImitateRenderManager->AddRenderList(m_PreviewDirectionalLight);

	m_ImitateRenderManager->AddRenderList(m_PreviewSky);
}


void AnimationEditor::ShowAnimationSequencer(float DeltaSeconds)
{
	float cursorX = ImGui::GetContentRegionAvail().x;
	float cursorY = ImGui::GetContentRegionAvail().y;
	ImGui::SetCursorPosX(cursorX * m_TargetViewRatioX);
	ImGui::SetCursorPosY(m_PropPosY);
	ImGui::BeginChild("NotifyDetails");
	m_NotifyInfoPropertySet->OnUpdate(DeltaSeconds);
	ImGui::EndChild();	

	
	ImGui::SetCursorPosY(cursorY );
	ImGui::SetCursorPosX(m_PropPosX);

	ImGui::BeginChild("NotifySeq");	
	m_Sequencer->OnUpdate(DeltaSeconds);	
	ImGui::EndChild();
}

void AnimationEditor::TargetViewHandleInput()
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
			m_Viewer->SetCameraFocusLocation(targetLoc);
			m_Viewer->SetCameraFocusMode(true);
		}
	} else if (ImGui::IsKeyReleased('F'))
	{
		m_Viewer->SetCameraFocusMode(false);
	}
}
