#include "EnginePCH.h"
#include "UnifromGridPanel.h"

UnifromGridPanel::UnifromGridPanel()
{
	bCanHaveMultipleChildren = true;
}

UnifromGridPanel::UnifromGridPanel(const UnifromGridPanel& Other)
	:Super(Other)
	, Left(Other.Left)
	, Right(Other.Right)
	, Bottom(Other.Bottom)
	, Top(Other.Top)
{
	bCanHaveMultipleChildren = true;
}

UnifromGridPanel::~UnifromGridPanel()
{

}

void UnifromGridPanel::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	//가장큰 Content 의 크기의 가장큰 Col , Row 
	SVector2 dSize = GetMaxRenderedSize();
		//GetMaxDesiredSize();
	DesiredSize = (dSize.x * (GetMaxColum()+1), dSize.y * (GetMaxRow()+1));
	
	
}

int32 UnifromGridPanel::GetMaxRow() const
{
	size_t slotSize = Slots.size();
	int32 MaxRow = 0;
	for(size_t i=0;i<slotSize;i++)
	{
		if (MaxRow < ((UniformGridSlot*)Slots[i])->Row)
			MaxRow = ((UniformGridSlot*)Slots[i])->Row;
	}
	return MaxRow;
}

int32 UnifromGridPanel::GetMaxColum() const
{
	size_t slotSize = Slots.size();
	int32 MaxCol = 0;
	for (size_t i = 0; i < slotSize; i++)
	{
		if (MaxCol < ((UniformGridSlot*)Slots[i])->Column)
			MaxCol = ((UniformGridSlot*)Slots[i])->Column;
	}
	return MaxCol;
}




UniformGridSlot* UnifromGridPanel::AddChildToUniformGrid(Widget* Content, int32 InRow /*= 0*/, int32 InColumn /*= 0*/)
{
	UniformGridSlot* GridSlot = (UniformGridSlot*)(Super::AddChild(Content));

	if (GridSlot != nullptr)
	{
		GridSlot->SetRow(InRow);
		GridSlot->SetColumn(InColumn);
	}

	return GridSlot;
}
