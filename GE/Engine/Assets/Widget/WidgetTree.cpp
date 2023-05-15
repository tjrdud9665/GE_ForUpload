#include "EnginePCH.h"
#include "WidgetTree.h"
#include "PanelWidget.h"
#include "UserWidget.h"

WidgetTree::WidgetTree()
{

}

WidgetTree::WidgetTree(const WidgetTree& Other)
	:Super(Other)
{
	if(Other.IsConstructed())
	{
		RootWidget = Other.RootWidget->Clone();
		//모든위젯을 Copy 하고 카피된 위젯을 계층구조에 맞게 넣어준다.

		


	}
}

WidgetTree::~WidgetTree()
{

}

void WidgetTree::BeginPlay()
{
	ForEachWidget([&](Widget* widget)
	{
		widget->BeginPlay();
	});
}

bool WidgetTree::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	

	return bResult;
}

void WidgetTree::PreUpdate(float DeltaSeconds)
{	
	ForEachWidget([&](Widget* widget)
	{
		widget->PreUpdate(DeltaSeconds);
	});
}

void WidgetTree::Update(float DeltaSeconds)
{
	ForEachWidget([&](Widget* widget)
	{
		widget->Update(DeltaSeconds);
	});
}

void WidgetTree::PostUpdate(float DeltaSeconds)
{
	ForEachWidget([&](Widget* widget)
	{
		widget->PostUpdate(DeltaSeconds);
	});
}

void WidgetTree::SetOwningScene(class Scene* InOwingScene)
{
	ForEachWidget([&](Widget* widget)
	{
		widget->SetOwningScene(InOwingScene);
	});
}

Widget* WidgetTree::CollisionMouse(class Input* pInput)
{
	Widget* colWidget = nullptr;

	//모든 위젯을돌기 때문에 
	//Report 하는방식아니라 각 위젯의 판단에 위임

	std::vector<Widget*> allWidgets;
	size_t widgetCnt = GetAllWidgets(OUT allWidgets);


	for(size_t i =0;i<widgetCnt;i++)
	{
		colWidget = allWidgets[i]->CollisionMouse(pInput);
	}

	//ForEachWidget([&](Widget* widget)
	//{
	//	colWidget = widget->CollisionMouse(pInput);

	//});

	return colWidget;
}

void WidgetTree::SetRootWidget(class Widget* InRootWidget)
{
	RootWidget = InRootWidget;			 

}	

Widget* WidgetTree::FindWidget(const std::string& InName) const
{
	Widget* FoundWidget = nullptr;

	ForEachWidget([&](Widget* widget)
	{
		if (widget->GetName() == InName)
		{
			FoundWidget = widget;
		}
	});

	return FoundWidget;
}

Widget* WidgetTree::FindWidgetElse(const std::string& InName) const
{
	size_t createdCnt = CreatedWidget.size();

	for(size_t i = 0;i<createdCnt;i++)
	{
		if(CreatedWidget[i]->GetName() == InName)
		{
			return CreatedWidget[i];
		}
	}
	return nullptr;
}

bool WidgetTree::RemoveWidget(Widget* InRemovedWidget)
{
	bool bRemoved = false;

	PanelWidget* InRemovedWidgetParent = InRemovedWidget->GetParent();
	if (InRemovedWidgetParent)
	{
		if (InRemovedWidgetParent->RemoveChild(InRemovedWidget))
		{
			bRemoved = true;
		}
	}
	// If the widget being removed is the root, null it out.
	else if (InRemovedWidget == RootWidget)
	{
		RootWidget = nullptr;
		bRemoved = true;
	}

	return bRemoved;
}

class PanelWidget* WidgetTree::FindWidgetParent(const Widget* InWidget, int32& OutChildIndex)
{
	PanelWidget* Parent = InWidget->GetParent();
	if (Parent != nullptr)
	{
		OutChildIndex = Parent->GetChildIndex(InWidget);
	} else
	{
		OutChildIndex = 0;
	}

	return Parent;
}

Widget* WidgetTree::FindWidgetChild(PanelWidget* InParentWidget, const std::string& ChildWidgetName, int32& OutChildIndex)
{
	OutChildIndex = INDEX_NONE;
	Widget* FoundChild = nullptr;

	if (InParentWidget)
	{
		const auto CheckWidgetFunc = [&FoundChild, ChildWidgetName](Widget* Widget)
		{
			if (!FoundChild && Widget->GetName() == ChildWidgetName)
			{
				FoundChild = Widget;
			}
		};

		// Check all the children of the given ParentWidget, but only track the index at the top level
		for (int32 ChildIdx = 0; ChildIdx < InParentWidget->GetChildrenCount(); ++ChildIdx)
		{
			CheckWidgetFunc(InParentWidget->GetChildAt(ChildIdx));

			if (!FoundChild)
			{
				ForWidgetAndChildren(InParentWidget->GetChildAt(ChildIdx), CheckWidgetFunc);
			}

			if (FoundChild)
			{
				OutChildIndex = ChildIdx;
				break;
			}
		}
	}

	return FoundChild;
}

int32 WidgetTree::FindChildIndex(const PanelWidget* InParentWidget, const Widget* InChildWidget)
{
	const Widget* CurrentWidget = InChildWidget;
	while (CurrentWidget)
	{
		const PanelWidget* NextParent = CurrentWidget->Slot ? CurrentWidget->Slot->Parent : nullptr;
		if (NextParent && NextParent == InParentWidget)
		{
			return NextParent->GetChildIndex(CurrentWidget);
		}

		CurrentWidget = NextParent;
	}

	return INDEX_NONE;
}

size_t WidgetTree::GetAllWidgets(std::vector<Widget*>& OutWidgets) const
{
	ForEachWidget([&OutWidgets](Widget* Widget)
	{
		OutWidgets.push_back(Widget);
	});

	return OutWidgets.size();
}

void WidgetTree::GetChildWidgets(class Widget* Parent, std::vector<Widget*>& Widgets)
{
	ForWidgetAndChildren(Parent, [&Widgets](Widget* widget)
	{
		Widgets.push_back(widget);
	});
}

void WidgetTree::GetOwnChildWidgets(class Widget* InParent, std::vector<Widget*>& Widgets)
{
	ForChildWidget(InParent, [&Widgets](Widget* widget)
	{
		Widgets.push_back(widget);
	});
}

void WidgetTree::ForEachWidget(std::function<void(Widget*)> Predicate) const
{
	if (RootWidget)
	{
		Predicate(RootWidget);

		ForWidgetAndChildren(RootWidget, Predicate);
	}

}

void WidgetTree::ForEachWidgetAndDescendants(std::function<void(Widget*)> Predicate) const
{
	if (RootWidget)
	{
		Predicate(RootWidget);

		ForWidgetAndChildren(RootWidget, [&Predicate](Widget* Child)
		{
			if(Child->GetClass()->IsChildOf(UserWidget::StaticClass()))			
			{
				UserWidget* UserWidgetChild = (UserWidget*)(Child);
				if (UserWidgetChild->m_WidgetTree)
				{
					UserWidgetChild->m_WidgetTree->ForEachWidgetAndDescendants(Predicate);
				}
			} else
			{
				Predicate(Child);
			}
		});
	}
}

void WidgetTree::ForWidgetAndChildren(Widget* InWidget, std::function<void(Widget*)> Predicate)
{

	// Search standard children.
	if(InWidget->GetClass()->IsChildOf(PanelWidget::StaticClass()))	
	{
		PanelWidget* PanelParent = (PanelWidget*)(InWidget);

		for (int32 ChildIndex = 0; ChildIndex < PanelParent->GetChildrenCount(); ChildIndex++)
		{
			Widget* ChildWidget = PanelParent->GetChildAt(ChildIndex);
			if (ChildWidget)
			{
				Predicate(ChildWidget);

				ForWidgetAndChildren(ChildWidget, Predicate);
			}
		}
	}
}

void WidgetTree::ForChildWidget(Widget* InWidget, std::function<void(Widget*)> Predicate)
{
	if (InWidget->GetClass()->IsChildOf(PanelWidget::StaticClass()))
	{
		PanelWidget* PanelParent = (PanelWidget*)(InWidget);

		for (int32 ChildIndex = 0; ChildIndex < PanelParent->GetChildrenCount(); ChildIndex++)
		{
			Widget* ChildWidget = PanelParent->GetChildAt(ChildIndex);
			if (ChildWidget)
			{
				Predicate(ChildWidget);				
			}
		}
	}
}
