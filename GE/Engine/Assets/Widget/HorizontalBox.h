#pragma once
#include "PanelWidget.h"
#include "HorizontalBoxSlot.h"

class HorizontalBox :public PanelWidget
{
	CLASS(HorizontalBox,PanelWidget)

protected:
	HorizontalBox();
	HorizontalBox(const HorizontalBox& Other);
	virtual ~HorizontalBox();

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual XClass* GetSlotClass() const
	{
		return HorizontalBoxSlot::StaticClass();
	}

};

