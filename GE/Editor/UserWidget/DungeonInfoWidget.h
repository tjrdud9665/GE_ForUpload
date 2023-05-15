#pragma once
#include "Assets/Widget\UserWidget.h"
class DungeonInfoWidget : public UserWidget
{
	CLASS(DungeonInfoWidget , UserWidget)

protected:
	DungeonInfoWidget();
	DungeonInfoWidget(const DungeonInfoWidget& Other);
	virtual~DungeonInfoWidget();

	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;


public:
	float UpdateInfo();
	
private:
	class CanvasPanel* m_RootPanel;
	class SizeBox* m_DungeonGageSize;
	class Overlay* m_ProgressOverlay;
	class ProgressBar* m_DungeonGageBar;
	class TextBlock* m_DungeonGageText;	

	class TextBlock* m_DungeonLevelText;
	class TextBlock* m_CurrentMonsterCountText;
	class TextBlock* m_RemainMonstCountText;

};

