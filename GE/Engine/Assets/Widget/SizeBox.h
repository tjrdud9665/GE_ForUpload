#pragma once
#include "ContentWidget.h"
#include "SizeBoxSlot.h"

class SizeBox :public ContentWidget
{
	CLASS(SizeBox,ContentWidget)

protected:
	SizeBox();
	SizeBox(const SizeBox& Other);
	virtual ~SizeBox();

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual XClass* GetSlotClass() const
	{
		return SizeBoxSlot::StaticClass();
	}

public:
	float GetWidthOverride() const;

	float GetHeightOverride() const;

protected:
	PROP(float, WidthOverride, DetailsEdit)
	float WidthOverride;

	PROP(float, HeightOverride, DetailsEdit)
	float HeightOverride;


};

