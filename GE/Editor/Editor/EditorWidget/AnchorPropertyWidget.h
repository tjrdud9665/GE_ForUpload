#pragma once
#include "PropertyWidget.h"
#include <Assets/Widget/CanvasPanelSlot.h>

class AnchorPropertyWidget :public PropertyWidget
{
	CLASS(AnchorPropertyWidget,PropertyWidget)
public:
	AnchorPropertyWidget();
	virtual ~AnchorPropertyWidget();

	virtual void Update(float DeltaSeconds) override;
	//virtual void PropertyGUI() override;

	TPropertyHandle<SAnchorData>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SAnchorData>*>(m_Prop);
	}

};

