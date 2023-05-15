#pragma once
#include "EditorWidget.h"
class CollisionEditorWidget :public EditorWidget
{
	CLASS(CollisionEditorWidget, EditorWidget)
	friend class SettingWindow;

public:
	CollisionEditorWidget();
	virtual ~CollisionEditorWidget();


	virtual void Construct(const std::string& InName = "") override;


	virtual void Update(float DeltaSeconds) override;

private:
	int32						m_SelectedIndex;

	std::string					m_SelectedChannelName;
	ECollision_Interaction		m_newChannelDefaultInteraction;
	std::string					m_newChannelName;


	std::string					m_SelectedProfileName;

	std::string					m_newProfileName;
	bool						m_newProfileEnabled;
	std::string					m_newProfileDesc;

	std::vector<ECollision_Interaction>	m_newProfileInteractions;


};

