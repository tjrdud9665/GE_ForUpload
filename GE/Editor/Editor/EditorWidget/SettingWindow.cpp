#include "PCH.h"
#include "SettingWindow.h"
#include "CollisionEditorWidget.h"
#include "InputEditorWidget.h"


SettingWindow::SettingWindow()
{
	m_seletedIdx = INDEX_NONE;
}

SettingWindow::~SettingWindow()
{

}

void SettingWindow::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
	
}



void SettingWindow::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
		int32 SettingWidgetSize = (int32)m_SettingWidgetes.size();

		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, ImGui::GetContentRegionAvail().y), false, window_flags);

		for (int32 i = 0; i < SettingWidgetSize; i++)
		{

			if (ImGui::Selectable(m_SettingWidgetes[i]->GetName().c_str(), i == m_seletedIdx))
			{
				m_seletedIdx = i;
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginGroup();
		if (m_seletedIdx != INDEX_NONE)
			m_SettingWidgetes[m_seletedIdx]->OnUpdate(DeltaSeconds);
		ImGui::EndGroup();

		EndWindow();
	}

}

void SettingWindow::Build()
{
	Super::Build();
	AddFlag(ImGuiWindowFlags_NoCollapse);
	AddFlag(ImGuiWindowFlags_NoDocking);

	CollisionEditorWidget* collisionEditor = CreateStandalone<CollisionEditorWidget>("Collision", this);
	InputEditorWidget* InputEditor = CreateStandalone<InputEditorWidget>("Input", this);

	m_SettingWidgetes.push_back(collisionEditor);
	m_SettingWidgetes.push_back(InputEditor);
}
