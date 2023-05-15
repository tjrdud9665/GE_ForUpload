#pragma once
#include "XObject/XObject.h"
#include "Widget.h"

class WidgetTree :public XObject
{
	CLASS(WidgetTree, XObject)
	friend class UserWidget;
protected:
	WidgetTree();
	WidgetTree(const WidgetTree& Other);
	virtual ~WidgetTree();

	virtual void BeginPlay();

	virtual bool Serialize(Archive& ar) override;

	virtual void PreUpdate(float DeltaSeconds) override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	void SetOwningScene(class Scene* InOwingScene);

	Widget* CollisionMouse(class Input* pInput);

public:	
	Widget* GetRootWidget() const
	{
		return RootWidget;
	}
	

	void SetRootWidget(class Widget* InRootWidget);

	Widget* FindWidget(const std::string& InName) const;		
		
	Widget* FindWidgetElse(const std::string& InName) const;
	
	template<typename WidgetT, typename = std::enable_if_t<std::is_base_of<Widget, WidgetT>::value>>
	WidgetT* FindWidget(const std::string& InName) const
	{
		return (WidgetT*)(FindWidget(InName));
	}

	/** Removes the widget from the hierarchy and all sub widgets. */
	bool RemoveWidget(Widget* InRemovedWidget);

	/** Gets the parent widget of a given widget, and potentially the child index. */
	static class PanelWidget* FindWidgetParent(const Widget* InWidget, int32& OutChildIndex);	

	/**
	 * Searches recursively through the children of the given ParentWidget to find a child widget of the given name.
	 * If successful, also gets the index the child ultimately occupies within the starting ParentWidget (INDEX_NONE otherwise)
	 */
	static Widget* FindWidgetChild(PanelWidget* InParentWidget, const std::string& ChildWidgetName, int32& OutChildIndex);

	/**
	 * Determines the child index of the given ParentWidget that the given ChildWidget ultimately occupies, accounting for nesting
	 * @return The child slot index within ParentWidget that ChildWidget ultimately occupies (INDEX_NONE if ChildWidget is not within ParentWidget at any level)
	 */
	static int32 FindChildIndex(const PanelWidget* InParentWidget, const Widget* InChildWidget);

	/** Gathers all the widgets in the tree recursively */
	size_t GetAllWidgets(std::vector<Widget*>& OutWidgets) const;

	/** Gathers descendant child widgets of a parent widget. */
	static void GetChildWidgets(class Widget* Parent, std::vector<Widget*>& Widgets);

	static void GetOwnChildWidgets(class Widget* InParent, std::vector<Widget*>& Widgets);

	/** Attempts to move a constructed Widget to another tree. Returns true on a successful move. */
	//static bool TryMoveWidgetToNewTree(Widget* widget, WidgetTree* DestinationTree);

	/**
	 * Iterates through all widgets including widgets contained in named slots, other than
	 * investigating named slots, this code does not dive into foreign WidgetTrees, as would exist
	 * inside another user widget.
	 */
	void ForEachWidget(std::function<void(Widget*)> Predicate) const;

	/**
	 * Iterates through all widgets including widgets contained in named slots, other than
	 * investigating named slots.  This includes foreign widget trees inside of other UserWidgets.
	 */
	void ForEachWidgetAndDescendants(std::function<void(Widget*)> Predicate) const;

	/**
	 * Iterates through all child widgets including widgets contained in named slots, other than
	 * investigating named slots, this code does not dive into foreign WidgetTrees, as would exist
	 * inside another user widget.
	 */
	static void ForWidgetAndChildren(Widget* InWidget, std::function<void(Widget*)> Predicate);

	static void ForChildWidget(Widget* InWidget, std::function<void(Widget*)> Predicate);

	/** Constructs the widget, and adds it to the tree. */
	template<typename WidgetT, typename = std::enable_if_t<std::is_base_of<Widget, WidgetT>::value>>
	WidgetT* ConstructWidget(TSubclassOf<Widget> WidgetClass = WidgetT::StaticClass(), const std::string& WidgetName = "" , bool EditorCreated =false)
	{		
		/*
		* 	if (WidgetClass->IsChildOf<UserWidget>())
		{
			return Cast<WidgetT>(CreateWidget(this, *WidgetClass, WidgetName));
		}
		*/
		std::string widgetName = WidgetName;
		if(widgetName.empty())
		{			
			widgetName = WidgetClass->GetName();
			
		}

		stringstream ss;
		auto iter = m_InstanceCounter.find(widgetName);

		if (iter == m_InstanceCounter.end())
			m_InstanceCounter.insert(std::make_pair(widgetName, 0));

		ss << widgetName << "_" << ++m_InstanceCounter[widgetName];

		WidgetT* widget = Globals::CreateX<WidgetT>(WidgetClass, this, ss.str());
		widget->m_IsCreatedEditor = EditorCreated;
		CreatedWidget.push_back(widget);
		return widget;
	}	

	
public:
	TSharedPtr<Widget> RootWidget;

	std::vector<Widget*>	CreatedWidget;

	std::unordered_map<std::string, uint64>	m_InstanceCounter;


	//Content-> SingleChild
	//
		

};

