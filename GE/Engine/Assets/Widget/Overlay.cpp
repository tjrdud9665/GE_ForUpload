#include "EnginePCH.h"
#include "Overlay.h"

Overlay::Overlay()
{
	bCanHaveMultipleChildren = true;
}

Overlay::Overlay(const Overlay& Other)
	:Super(Other)
{
	bCanHaveMultipleChildren = true;
}

Overlay::~Overlay()
{

}

void Overlay::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
}


void Overlay::PostUpdate(float DeltaSeconds)
{
	//가장 큰항목의 크기를 DesiredSize로 한다
	Super::PostUpdate(DeltaSeconds);	
	size_t slotSize = Slots.size();
	SVector2 maxSize = SVector2(1.0f);
	for (size_t i = 0; i < slotSize; i++)
	{
		Widget* c = Slots[i]->GetContent();
		if (maxSize.x <= c->GetDesiredSize().x)
			maxSize.x = c->GetDesiredSize().x;

		if (maxSize.y <= c->GetDesiredSize().y)
			maxSize.y = c->GetDesiredSize().y;
	}
	DesiredSize = maxSize;
}

OverlaySlot* Overlay::AddChildToOverlay(Widget* Content)
{
	return (OverlaySlot*)(Super::AddChild(Content));
}
