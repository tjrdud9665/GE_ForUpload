#pragma once
#include "PropertyWidget.h"
class Vector2PropertyWidget :public PropertyWidget
{
	CLASS(Vector2PropertyWidget, PropertyWidget)

public:
	Vector2PropertyWidget();
	virtual ~Vector2PropertyWidget();

	virtual void PropertyGUI() override;

	TPropertyHandle<SVector2>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SVector2>*>(m_Prop);
	}

};

