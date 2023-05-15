#pragma once
#include "PanelSlot.h"

class HorizontalBoxSlot :public PanelSlot
{
	CLASS(HorizontalBoxSlot,PanelSlot)

protected:
	HorizontalBoxSlot();
	HorizontalBoxSlot(const HorizontalBoxSlot& Other);
	virtual ~HorizontalBoxSlot();

	virtual void SetupGeometry(class Renderer* pRenderer) override;


protected:
	PROP(TEnum<EHorizontalAlignment>, HorizontalAlignment, DetailsEdit, "Slot")
		TEnum<EHorizontalAlignment> HorizontalAlignment;

	PROP(TEnum<EVerticalAlignment>, VerticalAlignment, DetailsEdit, "Slot")
		TEnum<EVerticalAlignment> VerticalAlignment;

	PROP(float, Left, DetailsEdit, "SlotPadding")
		float Left;

	PROP(float, Top, DetailsEdit, "SlotPadding")
		float Top;

	PROP(float, Right, DetailsEdit, "SlotPadding")
		float Right;

	PROP(float, Bottom, DetailsEdit, "SlotPadding")
		float Bottom;
};

