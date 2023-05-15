#include "PCH.h"
#include "SettingEditor.h"
#include "imgui.h"
#include "EditorWidget/SettingWindow.h"

 SettingEditor::SettingEditor()
{
	
}

 SettingEditor::SettingEditor(const SettingEditor& Other)
	 :Super(Other)
 {
	 //NO Call
 }

 SettingEditor::~SettingEditor()
 {

 }

 void SettingEditor::Construct(const std::string& InName /* = "" */)
 {
	 Super::Construct(InName);

	 m_SettingWindow = EditorWidget::CreateStandalone<SettingWindow>("SettingWindow",this);
	 m_SettingWindow->OnBuild();
	 m_SettingWindow->SetOpen(false);
 }

 void SettingEditor::ExpandViewsMenu()
 {
	 if (ImGui::BeginMenu("Settings"))
	 {
		 if (ImGui::MenuItem("Engine Settings", (const char*)0, m_SettingWindow->IsOpenedPtr())) {}

		 ImGui::EndMenu();
	 }

 }

 void SettingEditor::Update(float DeltaSeconds)
 {
	 m_SettingWindow->OnUpdate(DeltaSeconds);
 }

