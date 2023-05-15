#pragma once
#include "EditorWindow.h"

class SoundEditor : public EditorWindow
{
	CLASS(SoundEditor, EditorWindow)

public:
	SoundEditor();
	SoundEditor(const SoundEditor& Other);
	virtual ~SoundEditor();

	virtual void Build() override;

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Update(float DeltaSeconds) override;

	void SetEditSound(class Sound* InSound);

private:
	void ShowMenu();

private:
	class Sound* m_EditSound;

	class SoundWidget* m_SoundWidget;
};

