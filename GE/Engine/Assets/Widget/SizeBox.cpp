#include "EnginePCH.h"
#include "SizeBox.h"

SizeBox::SizeBox()
	:WidthOverride(32.0f)
	,HeightOverride(32.0f)
{

}

SizeBox::SizeBox(const SizeBox& Other)
	:Super(Other)
	, WidthOverride(Other.WidthOverride)
	, HeightOverride(Other.HeightOverride)
{

}

SizeBox::~SizeBox()
{

}

void SizeBox::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	DesiredSize = SVector2(WidthOverride, HeightOverride);
}

float SizeBox::GetWidthOverride() const
{
	return WidthOverride;
}

float SizeBox::GetHeightOverride() const
{
	return HeightOverride;
}
