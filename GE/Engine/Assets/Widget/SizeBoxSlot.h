#pragma once
#include "PanelSlot.h"

//Size¸¦ µ¤¾î¾¸

class SizeBoxSlot :public PanelSlot
{
	CLASS(SizeBoxSlot, PanelSlot)

protected:
	SizeBoxSlot();
	SizeBoxSlot(const SizeBoxSlot& Other);
	virtual ~SizeBoxSlot();

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

