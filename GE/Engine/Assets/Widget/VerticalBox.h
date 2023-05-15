#pragma once
#include "PanelWidget.h"
#include "VerticalBoxSlot.h"

class VerticalBox :public PanelWidget
{
	CLASS(VerticalBox,PanelWidget)

protected:
	VerticalBox();
	VerticalBox(const VerticalBox& Other);
	virtual ~VerticalBox();

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual XClass* GetSlotClass() const
	{
		return VerticalBoxSlot::StaticClass();
	}

};

