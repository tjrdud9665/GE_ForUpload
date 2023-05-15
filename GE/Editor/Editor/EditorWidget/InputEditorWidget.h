#pragma once
#include "EditorWidget.h"

class InputEditorWidget :public EditorWidget
{
	CLASS(InputEditorWidget, EditorWidget)
public:
	friend class SettingWindow;

public:
	InputEditorWidget();
	virtual ~InputEditorWidget();


	virtual void Construct(const std::string& InName = "") override;


	virtual void Update(float DeltaSeconds) override;


private:
	void ShowActionBinds();

	void ShowAxisBinds();

private:
	class Input* m_cachedInput;

	std::string			m_newActionName;

	uchar8				m_Selectedkey;

	std::string			m_SelectedActionName;


	bool				m_OpenCreateAction;

	bool				m_OpenAddActionKey;

	bool				m_OpenChangeActionKey;


	bool				m_OpenCreateAxis;




	std::string			m_newAxisName;

	std::string			m_SelectedAxisName;

	uchar8				m_SelectedAxisKey;


	float				m_SelectedAxisValue;

	bool				m_OpenAddAxisKey;

	bool				m_OpenChangeAxisKey;



};

