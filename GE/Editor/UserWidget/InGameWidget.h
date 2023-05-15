#pragma once
#include <Assets/Widget/UserWidget.h>

class InGameWidget :public UserWidget
{
	CLASS(InGameWidget, UserWidget)

protected:
	InGameWidget();
	InGameWidget(const InGameWidget& Other);
	virtual~InGameWidget();

	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;

	//0~~1
	float GetHealth();
	float GetSkill1Percent();
	float GetSkill2Percent();
	float GetSkill3Percent();
	float GetSkill4Percent();

private:
	class CanvasPanel* m_RootPanel;
	class SizeBox* m_HeathBarSize;
	class Overlay* m_ProgressOverlay;
	class ProgressBar* m_HealthBar;
	class TextBlock* m_HealthText;

	class SizeBox* m_Skill1SlotSize;
	class Overlay* m_Skill1Overlay;
	class ProgressBar* m_Skill1Bar;

	class SizeBox* m_Skill2SlotSize;
	class Overlay* m_Skill2Overlay;
	class ProgressBar* m_Skill2Bar;

	class SizeBox* m_Skill3SlotSize;
	class Overlay* m_Skill3Overlay;
	class ProgressBar* m_Skill3Bar;

	class SizeBox* m_Skill4SlotSize;
	class Overlay* m_Skill4Overlay;
	class ProgressBar* m_Skill4Bar;

};

