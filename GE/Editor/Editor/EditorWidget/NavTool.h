#pragma once
#include "EditorWindow.h"

class NavTool :public EditorWindow
{
	CLASS(NavTool, EditorWindow)

protected:
	NavTool();	
	virtual ~NavTool();

	virtual void Build() override;

private:
	class NavToolWidget* m_NavToolWidget;

	class PropertySet* m_NavPropertySet;
	
};

