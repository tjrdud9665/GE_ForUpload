#include "EnginePCH.h"
#include "ScrollBox.h"




ScrollBox::ScrollBox()
	:ScrollOffset(0.0f)
	,Orientation(EOrientation::Orient_Vertical)
{
	bCanHaveMultipleChildren = true;
}

ScrollBox::ScrollBox(const ScrollBox& Other)
	:Super(Other)
	, Orientation(Other.Orientation)
{
	bCanHaveMultipleChildren = true;
}

ScrollBox::~ScrollBox()
{

}



void ScrollBox::Render(class Renderer* pRenderer)
{		
	Super::Render(pRenderer);

}

void ScrollBox::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (m_MouseHovered)
	{
		int32 wheelDelta = m_CachedInput->ConsumeMouseWheelDelta();

		float wDelta = wheelDelta * 0.01f;		
		ScrollOffset = SMathUtils::Clamp(ScrollOffset += wDelta,0.0f,1.0f);
	}

}

void ScrollBox::PostUpdate(float DeltaSeconds)
{
	//Orientation 에따라 DesiredSize를 정한다
	Super::PostUpdate(DeltaSeconds);

	size_t slotSize = Slots.size();
	SVector2 maxSize = SVector2(1.0f);
	SVector2 sumSize = SVector2(0.0f);
	for (size_t i = 0; i < slotSize; i++)
	{
		Widget* c = Slots[i]->GetContent();
		if (maxSize.x <= c->GetDesiredSize().x)
			maxSize.x = c->GetDesiredSize().x;

		if (maxSize.y <= c->GetDesiredSize().y)
			maxSize.y = c->GetDesiredSize().y;

		sumSize += c->GetDesiredSize();
	}
	if(Orientation.Get() == EOrientation::Orient_Vertical)
	{
		DesiredSize.x = maxSize.x;
		DesiredSize.y = sumSize.y;
	}
	else
	{
		DesiredSize.x = sumSize.x;
		DesiredSize.y = maxSize.y;
	}

	
}

float ScrollBox::GetScrollOffset() const
{
	return ScrollOffset;
}

const TEnum<EOrientation>& ScrollBox::GetOrientation() const
{
	return Orientation;
}
