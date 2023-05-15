#include "PCH.h"
#include "ParticleEditor.h"
#include "PropertySet.h"
#include <imgui.h>
#include "Assets/Particle/ParticleAsset.h"
#include <GameFramework/Components/ParticleComponent.h>
#include <RenderManager.h>
#include "TargetViewer.h"
#include "CameraManager.h"

ParticleEditor::ParticleEditor()
{	
	m_PropPosY = 0.0f;	
}

ParticleEditor::ParticleEditor(const ParticleEditor& Other)
	:Super(Other)
{
	//Don't Copy
}

ParticleEditor::~ParticleEditor()
{
	SAFE_RELEASE(m_ImitateCameraManager);
	SAFE_RELEASE(m_ImitateRenderManager);

	if (m_ParticlePreviewComponent)
	{
		//
	}
}

void ParticleEditor::Build()
{
	Super::Build();


	m_TargetViewRatioX = 0.8f;
	m_Viewer = EditorWidget::AddNew<TargetViewer>("ParticlePreviewScene");

	m_Viewer->SetDesiredSizeRatioWidth(m_TargetViewRatioX);

	m_ImitateCameraManager = Globals::CreateEngineObject<CameraManager>();
	m_ImitateCameraManager->Initialize();
	m_ImitateCameraManager->AddRef();

	m_ImitateRenderManager = Globals::CreateEngineObject<RenderManager>();
	m_ImitateRenderManager->Initialize();
	m_ImitateRenderManager->AddRef();

	m_Viewer->SetTargetTexID(m_ImitateRenderManager->GetFinalTargetTextureID());
			
	
	m_ParticlePropertySet = EditorWidget::AddNew<PropertySet>("ParticlePorpertySet");

	m_ParticlePreviewComponent = Globals::CreateX<ParticleComponent>(ParticleComponent::StaticClass(), this, "PreviewParticle");

	AddFlag(ImGuiWindowFlags_NoDocking);
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoCollapse);
	
	
}

void ParticleEditor::SetTargetParticle(class ParticleAsset* InParticle)
{
	m_TargetParticle = InParticle;
	if(m_TargetParticle)
	{
		m_ParticlePreviewComponent->SetParticle(m_TargetParticle);
		m_ParticlePropertySet->SetPropertyTarget(m_TargetParticle);
	}

}

void ParticleEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (BeginWindow())
		{
			ImGui::PopStyleVar(1);			

			ShowMenu();
			UpdateEditSub(DeltaSeconds);

			CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
			m_ImitateCameraManager->PreUpdate(DeltaSeconds);
			m_ImitateCameraManager->Update(DeltaSeconds);
			m_ImitateCameraManager->PostUpdate(DeltaSeconds);

			m_PropPosY = ImGui::GetCursorPos().y;
			m_PropPosX = ImGui::GetCursorPos().x;
			m_Viewer->OnUpdate(DeltaSeconds);
			ShowParticleDetails(DeltaSeconds);
			ComposePreview();
			TargetViewHandleInput();
			g_Engine->Set<CameraManager>(oldCameraManager);
			EndWindow();
		}


	}
}

void ParticleEditor::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

}

void ParticleEditor::ShowParticleDetails(float DeltaSeconds)
{
	float cursorX = ImGui::GetContentRegionAvail().x;
	float cursorY = ImGui::GetContentRegionAvail().y;
	ImGui::SetCursorPosX(cursorX * m_TargetViewRatioX);
	ImGui::SetCursorPosY(m_PropPosY);
	ImGui::BeginChild("ParticleDetails");
	m_ParticlePropertySet->OnUpdate(DeltaSeconds);
	ImGui::EndChild();
}

void ParticleEditor::Render()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->Render();
	g_Engine->Set<CameraManager>(oldCameraManager);
}

void ParticleEditor::PreRender()
{
	CameraManager* oldCameraManager = g_Engine->Set<CameraManager>(m_ImitateCameraManager);
	m_ImitateRenderManager->PreRender();
	g_Engine->Set<CameraManager>(oldCameraManager);
}

void ParticleEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_TargetParticle)
					m_TargetParticle->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void ParticleEditor::UpdateEditSub(float DeltaSeconds)
{
	m_ParticlePreviewComponent->PreUpdate(DeltaSeconds);
	m_ParticlePreviewComponent->Update(DeltaSeconds);
	m_ParticlePreviewComponent->PostUpdate(DeltaSeconds);

}

void ParticleEditor::ComposePreview()
{
	if (m_ParticlePreviewComponent)
		m_ImitateRenderManager->AddRenderList(m_ParticlePreviewComponent);
}

void ParticleEditor::TargetViewHandleInput()
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
