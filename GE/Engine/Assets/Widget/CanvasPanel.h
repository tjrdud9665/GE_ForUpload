#pragma once
#include "PanelWidget.h"
#include "CanvasPanelSlot.h"

class CanvasPanelSlot;


class CanvasPanel :public PanelWidget
{
	CLASS(CanvasPanel,PanelWidget)

protected:
	CanvasPanel();
	CanvasPanel(const CanvasPanel& Other);
	virtual ~CanvasPanel();

	virtual XClass* GetSlotClass() const
	{
		return CanvasPanelSlot::StaticClass();
	}


	virtual void PostUpdate(float DeltaSeconds) override;

	
private:
	static bool SortZOrder(const PanelSlot* src, const PanelSlot* dest);

public:
	CanvasPanelSlot* AddChildToCanvas(Widget* Content);

	

};

