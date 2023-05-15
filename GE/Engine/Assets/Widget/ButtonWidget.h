#pragma once
#include "ContentWidget.h"
#include "ButtonSlot.h"

class ButtonWidget :public ContentWidget
{
	CLASS(ButtonWidget,ContentWidget)

protected:
	ButtonWidget();
	ButtonWidget(const ButtonWidget& Other);
	virtual ~ButtonWidget();

	virtual XClass* GetSlotClass() const
	{
		return ButtonSlot::StaticClass();
	}
	virtual void Render(class Renderer* pRenderer) override;

	virtual void PostUpdate(float DeltaSeconds) override;


	void SetupButtonGeometry(class Renderer* pRenderer);

public:
	template <typename T>
	void BindOnPressed(T* Obj, void(T::* Func)())
	{
		m_OnButtonPressed = std::bind(Func, Obj);
	}

	template <typename T>
	void BindOnReleased(T* Obj, void(T::* Func)())
	{
		m_OnButtonReleased = std::bind(Func, Obj);
	}


protected:
	//ButtonHovered
	virtual void OnMouseEntered(const SVector2& InMousePos) override;

	//ButtonLeaved
	virtual void OnMouseLeaved() override;

	virtual void OnMouseButtonDown(const SVector2& InMousePos, char MouseBtn) override;

	virtual void OnMouseButtonUp(const SVector2& InMousePos, char MouseBtn) override;

private:
	std::function<void(void)>	m_OnButtonPressed;
	std::function<void(void)>	m_OnButtonReleased;
	std::function<void(void)>	m_OnButtonClicked;		//Hovered+Pressed + Released

	std::function<void(void)>	m_OnButtonHovered;
	std::function<void(void)>	m_OnButtonUnHovered;

	std::vector<SVertex_PUV>	m_ButtonVertices;


private:
	PROP(TSharedPtr<TextureAsset>, NormalImage, DetailsEdit,"Button")
	TSharedPtr<TextureAsset> NormalImage;

	PROP(SLinearColor, NormalColor, DetailsEdit, "Button")
		SLinearColor	NormalColor;

	PROP(TSharedPtr<TextureAsset>, HoveredImage, DetailsEdit, "Button")
	TSharedPtr<TextureAsset> HoveredImage;

	PROP(SLinearColor, HoveredColor, DetailsEdit, "Button")
		SLinearColor	HoveredColor;

	PROP(TSharedPtr<TextureAsset>, PressedImage, DetailsEdit, "Button")
	TSharedPtr<TextureAsset> PressedImage;

	PROP(SLinearColor, PressedColor, DetailsEdit, "Button")
		SLinearColor	PressedColor;

};

