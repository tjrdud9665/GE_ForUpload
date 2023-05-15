#include "EnginePCH.h"
#include "CanvasPanel.h"

CanvasPanel::CanvasPanel()
{
	bCanHaveMultipleChildren = true;
}

CanvasPanel::CanvasPanel(const CanvasPanel& Other)
	:Super(Other)
{
	bCanHaveMultipleChildren = true;
}

CanvasPanel::~CanvasPanel()
{

}

void CanvasPanel::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	std::sort(Slots.begin(), Slots.end(), &CanvasPanel::SortZOrder);
}



bool CanvasPanel::SortZOrder(const PanelSlot* src, const PanelSlot* dest)
{
	return ((CanvasPanelSlot*)src)->ZOrder < ((CanvasPanelSlot*)dest)->ZOrder;
}

CanvasPanelSlot* CanvasPanel::AddChildToCanvas(Widget* Content)
{
	return (CanvasPanelSlot*)(Super::AddChild(Content));
}
