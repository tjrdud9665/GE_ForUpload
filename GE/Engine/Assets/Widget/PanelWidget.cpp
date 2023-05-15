#include "EnginePCH.h"
#include "PanelWidget.h"
#include "PanelSlot.h"
#include "CameraManager.h"
#include "GameFramework/Components/CameraComponent.h"
#include "WidgetTree.h"
#include "UserWidget.h"

PanelWidget::PanelWidget()	
	:bCanHaveMultipleChildren(true)
{

}

PanelWidget::PanelWidget(const PanelWidget& Other)
	:Super(Other)
	, bCanHaveMultipleChildren(Other.bCanHaveMultipleChildren)
{

	size_t slotSize = Other.Slots.size();
	Slots.resize(slotSize);
	for(size_t i=0;i<slotSize;i++)
	{
		Slots[i] = Other.Slots[i]->Clone();
		Slots[i]->Parent = this;		

		if (Slots[i]->Content->GetClass()->IsA(UserWidget::StaticClass()) ||
			Slots[i]->Content->GetClass()->IsChildOf(UserWidget::StaticClass()))
		{
			((UserWidget*)(Slots[i]->Content.Get()))->GetWidgetTree()->GetRootWidget()->Slot = Slots[i];			
		}

			

	}
	
}

PanelWidget::~PanelWidget()
{
	size_t slotSize = Slots.size();

	for(size_t i=0;i<slotSize;i++)
	{
		SAFE_DELETE(Slots[i]);
	}
}

void PanelWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
	
}

void PanelWidget::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
	size_t slotSize = Slots.size();

	for (size_t i = 0; i < slotSize; i++)
	{
		Slots[i]->PreUpdate(DeltaSeconds);

	}
}

void PanelWidget::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	size_t slotSize = Slots.size();

	for (size_t i = 0; i < slotSize; i++)
	{
		Slots[i]->Update(DeltaSeconds);

	}
}

void PanelWidget::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	size_t slotSize = Slots.size();

	for (size_t i = 0; i < slotSize; i++)
	{
		Slots[i]->PostUpdate(DeltaSeconds);

	}

}

int32 PanelWidget::GetChildrenCount() const
{
	return (int32)Slots.size();
}

Widget* PanelWidget::GetChildAt(int32 Index) const
{
	if(Slots.size() > Index)
	{
		return Slots[Index]->Content;	
	}	

	return nullptr;
}

std::vector<Widget*> PanelWidget::GetAllChildren() const
{
	std::vector<Widget*> Result;

	size_t slotSize = Slots.size();

	for(size_t i = 0;i< slotSize;i++)
	{
		Result.push_back(Slots[i]->Content);
	}

	return Result;
}

int32 PanelWidget::GetChildIndex(const Widget* Content) const
{
	const int32 ChildCount = GetChildrenCount();
	for (int32 ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
	{
		if (GetChildAt(ChildIndex) == Content)
		{
			return ChildIndex;
		}
	}
	return INDEX_NONE;
}

bool PanelWidget::HasChild(Widget* Content) const
{
	if (!Content)
	{
		return false;
	}
	return (Content->GetParent() == this);
}

bool PanelWidget::RemoveChildAt(int32 Index)
{
	if (Index < 0 || Index >= Slots.size())
	{
		return false;
	}

	PanelSlot* panelSlot = Slots[Index];	
	//if (panelSlot->Content)
	//{
	//	panelSlot->Content->Slot = nullptr;
	//	//SAFE_DELETE(panelSlot->Content->Slot);
	//}

	//SAFE_DELETE(Slots[Index]);
	Slots.erase(Slots.begin() + Index);
	//Slots.RemoveAt(Index);

	//OnSlotRemoved(panelSlot);
	
	SAFE_DELETE(panelSlot);

	return true;

}

class PanelSlot* PanelWidget::AddChild(Widget* Content)
{
	if (Content == nullptr)
	{
		return nullptr;
	}

	if (!bCanHaveMultipleChildren && GetChildrenCount() > 0)
	{
		return nullptr;
	}
	Content->AddRef();

	Content->RemoveFromParent();
	
	PanelSlot* panelSlot = Globals::CreateX<PanelSlot>(GetSlotClass(),this);
		/*NewObject<PanelSlot>(this, GetSlotClass(), NAME_None, NewObjectFlags);*/
	
	panelSlot->Content = Content;
	panelSlot->Parent = this;

	Content->Slot = panelSlot;	

	if(Content->GetClass()->IsA(UserWidget::StaticClass()) ||
	   Content->GetClass()->IsChildOf(UserWidget::StaticClass()))
	{
		((UserWidget*)(Content))->GetWidgetTree()->GetRootWidget()->Slot = panelSlot;
		
	}


	Slots.push_back(panelSlot);

	//OnSlotAdded(PanelSlot);

	//InvalidateLayoutAndVolatility();	
	Content->Release();
	return panelSlot;
}

bool PanelWidget::RemoveChild(Widget* Content)
{
	int32 ChildIndex = GetChildIndex(Content);
	if (ChildIndex != -1)
	{
		return RemoveChildAt(ChildIndex);
	}

	return false;
}

bool PanelWidget::HasAnyChildren() const
{
	return GetChildrenCount() > 0;
}

void PanelWidget::ClearChildren()
{
	for (int32 ChildIndex = GetChildrenCount() - 1; ChildIndex >= 0; ChildIndex--)
	{
		RemoveChildAt(ChildIndex);
	}
}

const std::vector<class PanelSlot*>& PanelWidget::GetSlots() const
{
	return Slots;
}

void PanelWidget::Render(class Renderer* pRenderer)
{
	//pRenderer->
	if (!Visible)
		return;

	size_t slotSize = Slots.size();

	for (size_t i = 0; i < slotSize; i++)
	{
		Slots[i]->Render(pRenderer);

	}	
	
}

bool PanelWidget::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	

	if(ar.IsLoading())
	{
		uint32 slotSize = 0;

		ar.BeginObject("slots");
		ar.SerializeArraySize(slotSize);		
		for (uint32 i = 0; i < slotSize; i++)
		{
			ar.BeginObject(i);
			std::string contentName;
			ar.Serialize("contentName", contentName);
			PanelSlot* slot = AddChild(GetOwner<WidgetTree>()->FindWidgetElse(contentName));
			if(!slot)
			{
				int32 idx = -1;
				WidgetTree::FindWidgetChild(this, contentName, OUT idx);
				if(idx != -1)
				{
					slot = Slots[idx];
				}
			}
			
			slot->Serialize(ar);			
			

			ar.EndObject();
		}
		ar.EndObject();
	}
	else
	{
		uint32 slotSize = (uint32)Slots.size();
	
		ar.BeginObject("slots");
		ar.SerializeArraySize(slotSize);
		Slots.resize(slotSize);
		for (uint32 i = 0; i < slotSize; i++)
		{
			ar.BeginObject(i);
			if(Slots[i]->Content)
			{
				std::string contentName = Slots[i]->Content->GetName();
				ar.Serialize("contentName", contentName);
				Slots[i]->Serialize(ar);
			}
			ar.EndObject();
		}
		ar.EndObject();
	}
	


	return bResult;
}

SVector2 PanelWidget::GetMaxDesiredSize() const
{
	size_t slotSize = Slots.size();
	SVector2 maxD = SVector2(0.0f);
	for (size_t i = 0; i < slotSize; i++)
	{
		SVector2 contentSize = Slots[i]->GetContent()->GetDesiredSize();
		if (maxD.x < contentSize.x)
			maxD.x = contentSize.x;
		if (maxD.y < contentSize.y)
			maxD.y = contentSize.y;
	}
	return maxD;
}

SVector2 PanelWidget::GetMaxRenderedSize() const
{
	size_t slotSize = Slots.size();
	SVector2 maxD = SVector2(0.0f);
	for (size_t i = 0; i < slotSize; i++)
	{
		SVector2 renderedSize = Slots[i]->GetContent()->GetRenderedSize();
		if (maxD.x < renderedSize.x)
			maxD.x = renderedSize.x;
		if (maxD.y < renderedSize.y)
			maxD.y = renderedSize.y;
	}
	return maxD;
}

SVector2 PanelWidget::GetAllRenderedSize() const
{
	size_t slotSize = Slots.size();
	SVector2 ret = SVector2(0.0f);
	for (size_t i = 0; i < slotSize; i++)
	{
		ret += Slots[i]->GetContent()->GetRenderedSize();
	
	}
	return ret;
}

SVector2 PanelWidget::GetSlotRenderedSize(int32 idx) const
{
	if (idx > Slots.size() || idx < 0)
		return SVector2(0, 0);

	return Slots[idx]->GetContent()->GetRenderedSize();
}

void PanelWidget::SetClipSpace(const SScissorsRect& InRect)
{
	size_t slotSize = Slots.size();

	for(size_t i =0;i<slotSize;i++)
	{
		Slots[i]->SetClipSpace(InRect);
	}
}


