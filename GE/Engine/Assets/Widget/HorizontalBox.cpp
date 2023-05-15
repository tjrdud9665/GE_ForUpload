#include "EnginePCH.h"
#include "HorizontalBox.h"

HorizontalBox::HorizontalBox()
{
	bCanHaveMultipleChildren = true;
}

HorizontalBox::HorizontalBox(const HorizontalBox& Other)
	:Super(Other)
{
	bCanHaveMultipleChildren = true;
}

HorizontalBox::~HorizontalBox()
{

}

void HorizontalBox::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	SVector2 dSize = GetMaxRenderedSize();
	//GetMaxDesiredSize();
	
	DesiredSize = SVector2(dSize.x * (GetChildrenCount()), dSize.y);
}
