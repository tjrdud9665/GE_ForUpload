#pragma once
#include "PropertyWidget.h"

class Vector3PropertyWidget :public PropertyWidget
{
	CLASS(Vector3PropertyWidget, PropertyWidget)

public:
	Vector3PropertyWidget();
	virtual ~Vector3PropertyWidget();

	virtual void PropertyGUI() override;

	TPropertyHandle<SVector3>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SVector3>*>(m_Prop);
	}


};

