#pragma once

#include "XObject/XObject.h"

class EditorWidget : public XObject
{
	CLASS(EditorWidget, XObject)
	using FWidgetContainer = std::vector<TSharedPtr<EditorWidget>>;

protected:
	EditorWidget();
	virtual ~EditorWidget();

	virtual void Construct(const std::string& InName = "") override;

public:
	EditorWidget* FindWidget(const std::string& InName) const;


public:
	/** Fixed Meathod */
	/** called AFTER parent widget is built */
	void OnBuild();

	void OnUpdate(float DeltaSeconds);



	/** Create child widgets here with  */
	virtual void Build() {}

	void ForceRebuild();


	virtual void Update(float DeltaSeconds) override;

	void SetWithSameline(bool InSameLine);



protected:
	void UpdateChilds(float DeltaSeconds);
	
	void SetDisableFlag(bool Disable);


public:
	/** Create widget from template type */
	template<typename WidgetType>
	WidgetType* New(const std::string& InName, XObject* Owner)
	{
		// Create the widget		
		WidgetType* widget = Globals::CreateX<WidgetType>(Owner, InName);
		return widget;
	}

	template<typename WidgetType>
	WidgetType* New(const std::string& InName, XClass* c)
	{
		// Create the widget
		WidgetType* widget = Globals::CreateX<WidgetType>(c, GetSelf(), InName);
		return widget;
	}

	EditorWidget* Add(EditorWidget* widget)
	{
		if (widget)
		{
			// Registry the widget
			EditorWidget* ptr = widget;	
			m_Children.push_back(widget);
			return ptr;
		}

		return nullptr;
	}

	template<typename WidgetType>
	WidgetType* AddNew(const std::string& InName)
	{
		WidgetType* ret = (WidgetType*)Add(New<WidgetType>(InName, GetSelf()));
		return ret;
	}

	template<typename WidgetType>
	static WidgetType* CreateStandalone(const std::string& InName, XObject* Owner)
	{
		// Create the widget
		WidgetType* widget = Globals::CreateX<WidgetType>(Owner, InName);
		widget->OnBuild();

		return widget;
	}

protected:
	void RemoveAllChildren();

	void RemoveWidget(class EditorWidget* InRemoval);

private:
	FWidgetContainer		m_Children;
	

	bool	m_Built;
	bool	m_Building;
	bool	m_DisableFlag;

	bool	m_WithSameLine;
	
};

//Create And Adding 
//