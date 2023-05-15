#include "PCH.h"
#include "SceneInfoEditor.h"
#include <Assets/SceneInfo/SceneInfo.h>
#include "PropertySet.h"
#include <imgui.h>

SceneInfoEditor::SceneInfoEditor()
{

}

SceneInfoEditor::~SceneInfoEditor()
{

}

void SceneInfoEditor::Build()
{
	Super::Build();

	m_PropertySet = EditorWidget::AddNew<PropertySet>("SceneInfoPropertySet");
	AddFlag(ImGuiWindowFlags_MenuBar);
}

void SceneInfoEditor::SetEditSceneInfo(class SceneInfo* InSceneInfo)
{
	m_TargetSceneInfo = InSceneInfo;
	m_PropertySet->SetPropertyTarget(m_TargetSceneInfo);
}

void SceneInfoEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		ShowMenu();
		UpdateChilds(DeltaSeconds);
		EndWindow();
	}

}

void SceneInfoEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_TargetSceneInfo)
					m_TargetSceneInfo->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}
