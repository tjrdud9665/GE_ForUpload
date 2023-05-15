#pragma once
#include "EditorWindow.h"
class MaterialEditor :public EditorWindow
{
	CLASS(MaterialEditor, EditorWindow)

public:
	MaterialEditor();
	MaterialEditor(const MaterialEditor& Other);
	virtual ~MaterialEditor();

	virtual void Build() override;

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Update(float DeltaSeconds) override;

	void SetEditMaterial(class Material* InEditMaterial);

	void ShowMenu();


private:
	TSharedPtr<class Material> m_EditMaterial;

	class PropertySet* m_MaterialPropertySet;


};

