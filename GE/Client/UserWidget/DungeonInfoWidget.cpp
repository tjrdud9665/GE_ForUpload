#include "PCH.h"
#include "DungeonInfoWidget.h"
#include <Assets/Widget/CanvasPanel.h>
#include <Assets/Widget/SizeBox.h>
#include <Assets/Widget/Overlay.h>
#include <Assets/Widget/WidgetTree.h>
#include <Assets/Widget/TextBlock.h>
#include <Assets/Widget/ProgressBar.h>
#include <GameFramework/GameObject/PlayerCharacter.h>
#include <Assets/Scene/Scene.h>
#include <Assets/SceneInfo/SceneInfo.h>
#include <GameplayAbility/CharacterAttributeSet.h>
#include "SceneInfo/CustomSceneInfo.h"

DungeonInfoWidget::DungeonInfoWidget()
{

}

DungeonInfoWidget::DungeonInfoWidget(const DungeonInfoWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{

		m_RootPanel = m_WidgetTree->FindWidget<CanvasPanel>("RootPanel_1");
		m_DungeonGageSize = m_WidgetTree->FindWidget<SizeBox>("DungeonGageBarSize_1");
		m_ProgressOverlay = m_WidgetTree->FindWidget<Overlay>("ProgressOverlay_1");
		m_DungeonGageBar = m_WidgetTree->FindWidget<ProgressBar>("DungeonGageBar_1");
		m_DungeonGageText = m_WidgetTree->FindWidget<TextBlock>("DungeonGageText_1");

		m_DungeonLevelText = m_WidgetTree->FindWidget<TextBlock>("DungeonLevelText_1");
		m_CurrentMonsterCountText = m_WidgetTree->FindWidget<TextBlock>("CurrentMonsterCountText_1");
		m_RemainMonstCountText = m_WidgetTree->FindWidget<TextBlock>("RemainMonstCountText_1");
	}
}

DungeonInfoWidget::~DungeonInfoWidget()
{

}

void DungeonInfoWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_RootPanel = m_WidgetTree->ConstructWidget<CanvasPanel>(CanvasPanel::StaticClass(), "RootPanel");
	m_DungeonGageSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "DungeonGageBarSize");
	m_ProgressOverlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "ProgressOverlay");
	m_DungeonGageBar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "DungeonGageBar");
	m_DungeonGageText = m_WidgetTree->ConstructWidget<TextBlock>(TextBlock::StaticClass(), "DungeonGageText");
	m_DungeonLevelText = m_WidgetTree->ConstructWidget<TextBlock>(TextBlock::StaticClass(), "DungeonLevelText");
	m_CurrentMonsterCountText = m_WidgetTree->ConstructWidget<TextBlock>(TextBlock::StaticClass(), "CurrentMonsterCountText");
	m_RemainMonstCountText = m_WidgetTree->ConstructWidget<TextBlock>(TextBlock::StaticClass(), "RemainMonstCountText");

	m_WidgetTree->SetRootWidget(m_RootPanel);
	m_RootPanel->AddChild(m_DungeonGageSize);
	m_DungeonGageSize->AddChild(m_ProgressOverlay);
	m_ProgressOverlay->AddChild(m_DungeonGageBar);
	m_ProgressOverlay->AddChild(m_DungeonGageText);

	m_RootPanel->AddChild(m_DungeonLevelText);
	m_RootPanel->AddChild(m_CurrentMonsterCountText);
	m_RootPanel->AddChild(m_RemainMonstCountText);
}

void DungeonInfoWidget::BeginPlay()
{
	Super::BeginPlay();

	m_DungeonGageBar->BindPecent(std::bind(&DungeonInfoWidget::UpdateInfo, this));
	
}

float DungeonInfoWidget::UpdateInfo()
{
	if (m_OwningScene)
	{
		CustomSceneInfo* scnInfo = dynamic_cast<CustomSceneInfo*>(m_OwningScene->GetSceneInfo());

		if (scnInfo)
		{
			int32 Percent = (int32)scnInfo->GetPercent();

			if (scnInfo->GetGuardianCount() > 0)
			{
				m_DungeonLevelText->SetText(u8"게이지 도달 시 보스 등장");
			}

			else
			{
				m_DungeonLevelText->SetText(u8"게이지 도달 시 던전 이동");
			}


			std::stringstream ss, ss1, ss2;
			ss << Percent << "%";

			m_DungeonGageText->SetText(ss.str());

			int32 MonsterCurrentCount, MonsterRemainCount;
		
			MonsterCurrentCount = scnInfo->GetCurrontMonsterCount();
			MonsterRemainCount = scnInfo->GetRemainMonsterCount();		

			ss1 << u8"몬스터 처치 수 : " << MonsterCurrentCount;	

			m_CurrentMonsterCountText->SetText(ss1.str());

			ss2 << u8"다음 단계까지 남은 몬스터 수 : " << MonsterRemainCount;

			m_RemainMonstCountText->SetText(ss2.str());

			return Percent / 100.f;
		}
	}

	return 0.0f;
}
