#pragma once
#include "Editor.h"


class SettingEditor :public Editor
{
	CLASS(SettingEditor, Editor)

protected:
	SettingEditor();
	SettingEditor(const SettingEditor& Other);
	virtual ~SettingEditor();

public:
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void ExpandViewsMenu() override;

	virtual void Update(float DeltaSeconds) override;

private:
	TSharedPtr<class SettingWindow>		m_SettingWindow;

};

