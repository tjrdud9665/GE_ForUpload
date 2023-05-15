#include "PCH.h"
#include "TargetViewer.h"
#include "Renderer/Renderer.h"
#include "RenderManager.h"
#include "imgui.h"
#include "CameraControlWidget.h"
#include "EditorWindow.h"
#include "imgui_internal.h"


TargetViewer::TargetViewer()
{
	m_cahcedRenderManager = nullptr;
	m_cachedRenderer = nullptr;
	m_TagetTexID = INDEX_NONE;
	m_DesiredSizeRatio = SVector2(1.0f);
}

TargetViewer::~TargetViewer()
{

}

void TargetViewer::Build()
{	
	m_cahcedRenderManager = g_Engine->Get<RenderManager>();
	m_cachedRenderer = g_Engine->Get<Renderer>();
	m_CameraController = EditorWidget::AddNew<CameraControlWidget>("ViewerCamera");
}

void TargetViewer::Update(float DeltaSeconds)
{
	D3DTexture* tex = m_cachedRenderer->GetTextureObject(m_TagetTexID);
	
	
	ImVec2 size = ImGui::GetContentRegionAvail();

	//float q = size.x / size.y;
	float w = size.x * m_DesiredSizeRatio.x / (float)tex->GetWidth();
	float h = size.y * m_DesiredSizeRatio.y / (float)tex->GetHeight();

	m_TargetTexSize = SVector2(tex->GetWidth() * w , tex->GetHeight() * h);

	ImVec2 imSize = ImVec2(m_TargetTexSize.x,m_TargetTexSize.y);

	ImGui::Image(tex->m_SRV, imSize);	

	if(ImGui::IsItemHovered())
	{
		ImGui::GetCurrentWindow()->Flags |= ImGuiWindowFlags_NoMove;		
	}
	
	UpdateChilds(DeltaSeconds);

}

void TargetViewer::SetTargetTexID(FTextureID InTexID)
{
	m_TagetTexID = InTexID;
}

void TargetViewer::SetDesiredSizeRatioWidth(float Width)
{
	m_DesiredSizeRatio.x = Width;
}

void TargetViewer::SetDesiredSizeRatioHeight(float Height)
{
	m_DesiredSizeRatio.y = Height;
}

void TargetViewer::SetCameraFocusMode(bool InFocusMode)
{
	m_CameraController->SetFocusMode(InFocusMode);
}

void TargetViewer::SetCameraFocusLocation(const SVector3& InLocation)
{
	m_CameraController->SetFocusLocation(InLocation);
}
