#pragma once
#include <Assets/Widget/UserWidget.h>

class BossWidget : public UserWidget
{
	CLASS(BossWidget, UserWidget)

protected:
	BossWidget();
	BossWidget(const BossWidget& Other);
	virtual~BossWidget();

	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;

	//0~~1
	float GetHealth();

private:
	class CanvasPanel* m_RootPanel;
	class SizeBox* m_HeathBarSize;
	class Overlay* m_ProgressOverlay;
	class ProgressBar* m_HealthBar;
	class TextBlock* m_HealthText;
};

