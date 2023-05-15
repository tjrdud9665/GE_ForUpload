#pragma once
#include <Assets/Widget/UserWidget.h>

class CustomUserWidget :public UserWidget
{
	CLASS(CustomUserWidget,UserWidget)

protected:
	CustomUserWidget();
	CustomUserWidget(const CustomUserWidget& Other);
	virtual~CustomUserWidget();

	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;


private:
	void OnButtonPressed();
	

private:
	class SizeBox* m_RootSizeBox;
	class ButtonWidget* m_Button;
	class Overlay* m_ButtonOverlay;


};

