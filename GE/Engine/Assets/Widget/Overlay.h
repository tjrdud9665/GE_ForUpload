#pragma once
#include "PanelWidget.h"
#include "OverlaySlot.h"

class Overlay : public PanelWidget
{
	CLASS(Overlay,PanelWidget)

protected:
	Overlay();
	Overlay(const Overlay& Other);
	virtual ~Overlay();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual XClass* GetSlotClass() const
	{
		return OverlaySlot::StaticClass();
	}	
	
	
	OverlaySlot* AddChildToOverlay(Widget* Content);

	

};

