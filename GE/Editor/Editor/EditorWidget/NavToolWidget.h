#pragma once
#include "EditorWidget.h"

class NavToolWidget :public EditorWidget
{
	CLASS(NavToolWidget, EditorWidget)

public:
	NavToolWidget();
	virtual ~NavToolWidget();


	virtual void Update(float DeltaSeconds) override;

	virtual void Build() override;


private:
	class NavigationSystem* m_cahcedNavSystem;




};

