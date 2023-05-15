#include "EnginePCH.h"
#include "VerticalBox.h"

VerticalBox::VerticalBox()
{
	bCanHaveMultipleChildren = true;
}

VerticalBox::VerticalBox(const VerticalBox& Other)
	:Super(Other)
{
	bCanHaveMultipleChildren = true;
}

VerticalBox::~VerticalBox()
{

}

void VerticalBox::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	SVector2 dSize = GetMaxRenderedSize();
	//GetMaxDesiredSize();

	DesiredSize = SVector2(dSize.x, dSize.y *(GetChildrenCount()));	
}
