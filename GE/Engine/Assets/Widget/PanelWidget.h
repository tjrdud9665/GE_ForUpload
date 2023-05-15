#pragma once
#include "Widget.h"
#include "PanelSlot.h"

class PanelWidget :public Widget
{
	CLASS(PanelWidget, Widget)
	friend class WidgetTree;

protected:
	PanelWidget();
	PanelWidget(const PanelWidget& Other);
	virtual ~PanelWidget();

protected:
	virtual void Construct(const std::string& InName = "") override;

	virtual void PreUpdate(float DeltaSeconds) override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

public:
	int32 GetChildrenCount() const;

	Widget* GetChildAt(int32 Index) const;

	std::vector<Widget*> GetAllChildren() const;

	int32 GetChildIndex(const Widget* Content) const;

	bool HasChild(Widget* Content) const;

	bool RemoveChildAt(int32 Index);

	class PanelSlot* AddChild(Widget* Content);

	virtual XClass* GetSlotClass() const
	{
		return PanelSlot::StaticClass();
	}

	bool RemoveChild(Widget* Content);

	bool HasAnyChildren() const;

	virtual void ClearChildren();

	const std::vector<class PanelSlot*>& GetSlots() const;

	bool CanHaveMultipleChildren() const
	{
		return bCanHaveMultipleChildren;
	}

	bool CanAddMoreChildren() const
	{
		return CanHaveMultipleChildren() || GetChildrenCount() == 0;
	}


	virtual void Render(class Renderer* pRenderer) override;

	virtual bool Serialize(Archive& ar) override;
	
	SVector2 GetMaxDesiredSize() const;

	SVector2 GetMaxRenderedSize() const;

	SVector2 GetAllRenderedSize() const;
	
	SVector2 GetSlotRenderedSize(int32 idx) const;

	virtual void SetClipSpace(const SScissorsRect& InRect) override;
protected:

	//As 
	std::vector<class PanelSlot*> Slots;

	/** Can this panel allow for multiple children? */
	bool bCanHaveMultipleChildren;

public:
	//FRenderTargetID			m_PanelTarget;
	

};

