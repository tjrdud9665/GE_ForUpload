#pragma once
#include <Assets/Widget/UserWidget.h>

class DungeonUIWidget : public UserWidget
{
	CLASS(DungeonUIWidget, UserWidget)

protected:
	DungeonUIWidget();
	DungeonUIWidget(const DungeonUIWidget& Other);
	virtual~DungeonUIWidget();

	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;

private:
	void OnStartButtonPressed();

	void OnEndButtonPressed();


private:
	class CanvasPanel* m_RootPanel;

	class SizeBox* m_StartSizeBox;
	class ButtonWidget* m_StartButton;

	class SizeBox* m_EndSizeBox;
	class ButtonWidget* m_EndButton;
};

