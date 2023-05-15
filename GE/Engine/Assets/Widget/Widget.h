#pragma once
#include "../AssetData.h"
#include "../Mesh/Mesh.h"



/*
*
				Widget
PanelWidget (bCanHaveMultipleChildren) 복수자손을 가질수 있는지 아닌지에 대한 지정이가능
(단일자손)ContentWidget				(복수자손)CanvaPanel
*/
class Widget :public AssetData
{
	CLASS(Widget, AssetData)
	friend class PanelWidget;
	friend class WidgetTree;
	friend class WidgetEditor;
	friend class PanelSlot;
	friend class UserWidget;
	friend class UIManager;

protected:
	Widget();
	Widget(const Widget& Other);
	virtual ~Widget();


public:
	virtual void BeginPlay();

	/* */
	virtual void SetOwningScene(class Scene* InScene);

	virtual void Construct(const std::string& InName /* = "" */) override;
		

	virtual void PreUpdate(float DeltaSeconds) override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;
	
	
	
	void SetRenderTransform(SWidgetTransform InTransform);

	const SVector2& GetDesiredSize() const;

	const SVector2& GetRenderedSize() const;
	

	class PanelWidget* GetParent() const;

	class PanelWidget* GetRoot() const;

	/*DragDrop을 수행할때 TargetWidget의 RefCnt를  먼저 올리고 제거할것 그렇지않으면 Content가 삭제됨*/
	virtual void RemoveFromParent();

	virtual void Render(class Renderer* pRenderer);	

	class PanelSlot* GetPanelSlot() const;

	void SetDesiredSize(const SVector2& InDesiredSize);

	SVector2		GetLeftTop();
	SVector2		GetRightBottom();
	
	virtual void SetClipSpace(const SScissorsRect& InRect);

	void SetRenderedSize(const SVector2& InRenderedSize);


	virtual Widget* CollisionMouse(class Input* pInput);

	void MouseEntered(const SVector2& InMousePos);

	virtual void OnMouseEntered(const SVector2& InMousePos) {}

	void MouseLeaved();

	virtual void OnMouseLeaved() {}	

	//void MouseMove();

	//virtual void OnMouseMove();

	void MouseButtonDown(const SVector2& InMousePos, char MouseBtn);
	virtual void OnMouseButtonDown(const SVector2& InMousePos, char MouseBtn) {}


	void MouseButtonUp(const SVector2& InMousePos, char MouseBtn);
	virtual void OnMouseButtonUp(const SVector2& InMousePos, char MouseBtn) {}

	void SetHovered(bool InHovered);
	

protected:	

	class Scene* m_OwningScene;

	class PanelSlot* Slot;

	PROP(SWidgetTransform,RenderTransform,DetailsEdit)
	SWidgetTransform RenderTransform;

	SVector2 RenderTransformPivot;

	PROP(SVector2, DesiredSize, DetailsView | Transient)
	SVector2		DesiredSize;

	PROP(SVector2, RenderedSize, DetailsView | Transient)
	SVector2	RenderedSize;
	//ClippingOpt
	//EWidgetClipping Clipping;
		
	//Visibility -> Visible Collapsed HitTestInvisible SelfHitTestInvisible
	//ESlateVisibility Visibility; 

	PROP(float, RenderOpacity,DetailsEdit)
	float RenderOpacity;

	PROP(SLinearColor, TintColor, DetailsEdit)
	SLinearColor	TintColor;

	PROP(bool, Visible, DetailsEdit)
	bool Visible;

	class Renderer* m_CachedRenderer;
	class Input* m_CachedInput;

	//로드시에 중복생성방지
	bool		m_IsCreatedEditor;
	
public:
	class UIManager* m_CachedManager;
	
protected:
	std::function<void(void)>	m_OnMouseEntered;
	std::function<void(void)>	m_OnMouseLeaved;
	
	bool						m_MouseHovered;
	SVector2					m_HoveredPos;
	bool						m_PreHovered;

	bool						m_Pressed;
public:
	template <typename T>
	void BindOnHovered(T* Obj, void(T::* Func)())
	{
		m_OnMouseEntered = std::bind(Func, Obj);
	}

	template <typename T>
	void BindOnLeaved(T* Obj, void(T::* Func)())
	{
		m_OnMouseLeaved = std::bind(Func, Obj);
	}

};

