#pragma once
#include "PanelWidget.h"
#include "UniformGridSlot.h"

class UnifromGridPanel :public PanelWidget
{
	CLASS(UnifromGridPanel, PanelWidget)
	friend class UniformGridSlot;
protected:
	UnifromGridPanel();
	UnifromGridPanel(const UnifromGridPanel& Other);
	virtual ~UnifromGridPanel();

	virtual XClass* GetSlotClass() const
	{
		return UniformGridSlot::StaticClass();
	}


	virtual void PostUpdate(float DeltaSeconds) override;

	
	int32 GetMaxRow() const;

	int32 GetMaxColum() const;
	


	


public:
	UniformGridSlot* AddChildToUniformGrid(Widget* Content, int32 InRow = 0, int32 InColumn = 0);

private:
	PROP(float, Left, DetailsEdit , "SlotPadding")
	float Left;

	PROP(float, Top, DetailsEdit, "SlotPadding")
	float Top;

	PROP(float, Right, DetailsEdit, "SlotPadding")
	float Right;

	PROP(float, Bottom, DetailsEdit, "SlotPadding")
	float Bottom;

};

