#pragma once
#include "PropertyWidget.h"
class ColorPropertyWidget :public PropertyWidget
{
	CLASS(ColorPropertyWidget, PropertyWidget)


public:
	ColorPropertyWidget();
	virtual ~ColorPropertyWidget();

	virtual void PropertyGUI() override;

	TPropertyHandle<SLinearColor>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SLinearColor>*>(m_Prop);
	}

};

