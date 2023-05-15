#pragma once
#include "PropertyWidget.h"

class WidgetTransformWidget :public PropertyWidget
{
	CLASS(WidgetTransformWidget,PropertyWidget)

public:
	WidgetTransformWidget();
	virtual ~WidgetTransformWidget();

	virtual void Update(float DeltaSecondS) override;

protected:
	TPropertyHandle<SWidgetTransform>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SWidgetTransform>*>(m_Prop);
	}

};

