#pragma once
#include "PanelSlot.h"

/*
* 
*/

class UniformGridSlot : public PanelSlot
{
	CLASS(UniformGridSlot, PanelSlot)
	friend class UnifromGridPanel;

protected:
	UniformGridSlot();
	UniformGridSlot(const UniformGridSlot& Other);
	virtual ~UniformGridSlot();

	virtual void SetupGeometry(class Renderer* pRenderer) override;
	

	void SetRow(int32 InRow);

	void SetColumn(int32 InCol);

protected:
	PROP(TEnum<EHorizontalAlignment>, HorizontalAlignment, DetailsEdit,"Slot")
	TEnum<EHorizontalAlignment> HorizontalAlignment;

	
	PROP(TEnum<EVerticalAlignment>, VerticalAlignment, DetailsEdit, "Slot")
	TEnum<EVerticalAlignment> VerticalAlignment;

	
	PROP(int32, Row, DetailsEdit, "Slot")
	int32 Row;
	
	PROP(int32,Column,DetailsEdit,"Slot")
	int32 Column;
};

