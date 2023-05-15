#include "PCH.h"
#include "SoundEditor.h"
#include <Assets/Sound/Sound.h>
#include "SoundWidget.h"

SoundEditor::SoundEditor()
{

}

SoundEditor::SoundEditor(const SoundEditor& Other)
	:Super(Other)
{
	//Dont Copy
}

SoundEditor::~SoundEditor()
{

}

void SoundEditor::Build()
{
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoDocking);
}

void SoundEditor::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_SoundWidget = AddNew<SoundWidget>("SoundWidget");

}

void SoundEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		if (m_EditSound)
		{
			if (!m_EditSound->IsValidate())
			{
				m_EditSound = nullptr;
				return;
			}
			BeginWindow();
			ShowMenu();

			ImGui::Text(m_EditSound->GetName().c_str());

			UpdateChilds(DeltaSeconds);
			


			EndWindow();

		}
	}
}

void SoundEditor::SetEditSound(Sound* InSound)
{
	m_EditSound = InSound;
	m_SoundWidget->SetEditSound(InSound);
}

void SoundEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_EditSound)
					m_EditSound->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}
