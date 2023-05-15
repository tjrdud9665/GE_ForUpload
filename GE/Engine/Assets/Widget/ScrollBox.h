#pragma once
#include "PanelWidget.h"
#include "ScrollBoxSlot.h"

//Slot 들을 보여주는데 일정 크기보다 커지면 스크롤하게끔 유도

class ScrollBox :public PanelWidget
{
	CLASS(ScrollBox, PanelWidget)
	friend class ScrollBoxSlot;
protected:
	ScrollBox();
	ScrollBox(const ScrollBox& Other);
	virtual ~ScrollBox();
	

	virtual void Render(class Renderer* pRenderer) override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	float GetScrollOffset() const;

	virtual XClass* GetSlotClass() const
	{
		return ScrollBoxSlot::StaticClass();
	}

	const TEnum<EOrientation>&		GetOrientation() const;


	

private:
	PROP(TEnum<EOrientation>, Orientation, DetailsEdit)
	TEnum<EOrientation> Orientation;


	//0~~1
	PROP(float, ScrollOffset, DetailsEdit)
	float ScrollOffset;
};

