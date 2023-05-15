#pragma once
#include "EditorWindow.h"

class SettingWindow :public EditorWindow
{
	CLASS(SettingWindow, EditorWindow)
	friend class SettingEditor;
	using FWidgetContainer = std::vector<TSharedPtr<EditorWidget>>;
public:
	SettingWindow();
	virtual ~SettingWindow();



	virtual void Construct(const std::string& InName = "") override;


	virtual void Update(float DeltaSeconds) override;

	virtual void Build() override;


private:
	FWidgetContainer	m_SettingWidgetes;
	int32				m_seletedIdx;
};

