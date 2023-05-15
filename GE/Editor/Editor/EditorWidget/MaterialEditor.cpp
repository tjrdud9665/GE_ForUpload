#include "PCH.h"
#include "MaterialEditor.h"
#include "Assets/Material/Material.h"
#include "PropertySet.h"

MaterialEditor::MaterialEditor()
{

}

MaterialEditor::MaterialEditor(const MaterialEditor& Other)
	:Super(Other)
{
	//DOnt Copy
}

MaterialEditor::~MaterialEditor()
{

}

void MaterialEditor::Build()
{
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoDocking);
}

void MaterialEditor::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_MaterialPropertySet = AddNew<PropertySet>("MaterialPropertySet");

}

void MaterialEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		if (m_EditMaterial)
		{
			if (!m_EditMaterial->IsValidate())
			{
				m_EditMaterial = nullptr;
				return;
			}
			BeginWindow();
			ShowMenu();

			ImGui::Text(m_EditMaterial->GetName().c_str());

			UpdateChilds(DeltaSeconds);			

			EndWindow();

		}
	}
}

void MaterialEditor::SetEditMaterial(class Material* InEditMaterial)
{
	m_EditMaterial = InEditMaterial;
	m_MaterialPropertySet->SetPropertyTarget(m_EditMaterial);
}

void MaterialEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_EditMaterial)
					m_EditMaterial->Save();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	
}
