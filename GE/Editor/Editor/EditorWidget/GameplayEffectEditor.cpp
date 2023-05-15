#include "PCH.h"
#include "GameplayEffectEditor.h"
#include <Assets/GameplayEffect/GameplayEffect.h>
#include "PropertySet.h"

GameplayEffectEditor::GameplayEffectEditor()
{

}

GameplayEffectEditor::GameplayEffectEditor(const GameplayEffectEditor& Other)
	:Super(Other)
{

}

GameplayEffectEditor::~GameplayEffectEditor()
{

}

void GameplayEffectEditor::Build()
{
	Super::Build();
	m_EffectProperty = EditorWidget::AddNew<PropertySet>("EffectPropertySet");
	AddFlag(ImGuiWindowFlags_MenuBar);
}

void GameplayEffectEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		ShowMenu();
		UpdateChilds(DeltaSeconds);
		EndWindow();
	}
}

void GameplayEffectEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_EditEffect)
					m_EditEffect->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void GameplayEffectEditor::SetTargetGameplayEffect(class GameplayEffect* InGameplayEffect)
{
	m_EditEffect = InGameplayEffect;

	m_EffectProperty->SetPropertyTarget(m_EditEffect);
}
