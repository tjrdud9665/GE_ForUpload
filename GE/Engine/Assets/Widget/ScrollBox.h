#pragma once
#include "PanelWidget.h"
#include "ScrollBoxSlot.h"

//Slot ���� �����ִµ� ���� ũ�⺸�� Ŀ���� ��ũ���ϰԲ� ����

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

