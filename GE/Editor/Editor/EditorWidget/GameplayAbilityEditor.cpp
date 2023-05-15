#include "PCH.h"
#include "GameplayAbilityEditor.h"
#include "PropertySet.h"
#include <Assets/GameplayAbility/GameplayAbility.h>

GameplayAbilityEditor::GameplayAbilityEditor()
{

}

GameplayAbilityEditor::GameplayAbilityEditor(const GameplayAbilityEditor& Other)
	:Super(Other)
{

}

GameplayAbilityEditor::~GameplayAbilityEditor()
{

}

void GameplayAbilityEditor::Build()
{
	Super::Build();
	m_AbilityProperty = EditorWidget::AddNew<PropertySet>("AbilityPropertySet");
	AddFlag(ImGuiWindowFlags_MenuBar);
}

void GameplayAbilityEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		ShowMenu();
		UpdateChilds(DeltaSeconds);
		EndWindow();
	}
}

void GameplayAbilityEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_EditAbility)
					m_EditAbility->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void GameplayAbilityEditor::SetTargetGameplayAbility(class GameplayAbility* InGameplayAbility)
{
	m_EditAbility = InGameplayAbility;

	m_AbilityProperty->SetPropertyTarget(m_EditAbility);
}
