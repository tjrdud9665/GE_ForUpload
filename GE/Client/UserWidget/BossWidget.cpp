#include "PCH.h"
#include "BossWidget.h"
#include <Assets/Widget/CanvasPanel.h>
#include <Assets/Widget/SizeBox.h>
#include <Assets/Widget/Overlay.h>
#include <Assets/Widget/WidgetTree.h>
#include <Assets/Widget/TextBlock.h>
#include <Assets/Widget/ProgressBar.h>
#include <Assets/Scene/Scene.h>
#include "GameplayAbility/CharacterAttributeSet.h"
#include <ObjectManager.h>
#include "../GameObject/BossMonster.h"

BossWidget::BossWidget()
{
}

BossWidget::BossWidget(const BossWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{

		m_RootPanel = m_WidgetTree->FindWidget<CanvasPanel>("RootPanel_1");
		m_HeathBarSize = m_WidgetTree->FindWidget<SizeBox>("HeathBarSize_1");
		m_ProgressOverlay = m_WidgetTree->FindWidget<Overlay>("ProgressOverlay_1");
		m_HealthBar = m_WidgetTree->FindWidget<ProgressBar>("HealthBar_1");
		m_HealthText = m_WidgetTree->FindWidget<TextBlock>("HealthText_1");
	}
}

BossWidget::~BossWidget()
{
}

void BossWidget::Construct(const std::string& InName)
{
	Super::Construct(InName);



	m_RootPanel = m_WidgetTree->ConstructWidget<CanvasPanel>(CanvasPanel::StaticClass(), "RootPanel");
	m_HeathBarSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "HeathBarSize");
	m_ProgressOverlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "ProgressOverlay");
	m_HealthBar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "HealthBar");
	m_HealthText = m_WidgetTree->ConstructWidget<TextBlock>(TextBlock::StaticClass(), "HealthText");

	m_WidgetTree->SetRootWidget(m_RootPanel);
	m_RootPanel->AddChild(m_HeathBarSize);
	m_HeathBarSize->AddChild(m_ProgressOverlay);
	m_ProgressOverlay->AddChild(m_HealthBar);
	m_ProgressOverlay->AddChild(m_HealthText);
}

void BossWidget::BeginPlay()
{
	Super::BeginPlay();

	m_HealthBar->BindPecent(std::bind(&BossWidget::GetHealth, this));
}

float BossWidget::GetHealth()
{
	if (m_OwningScene)
	{
		BossMonster* Boss = m_OwningScene->GetObjectManager()->FindGameObjectByClass<BossMonster>();

		if (Boss)
		{
			CharacterAttributeSet* targetAttSet = Boss->GetAttributeSetAs<CharacterAttributeSet>();
			int32 curHealth = (int32)targetAttSet->GetHealth();
			int32 maxHealth = (int32)std::max(targetAttSet->GetMaxHealth(), 1.0f);
			if (m_HealthText)
			{
				std::stringstream ss;
				ss << curHealth << "/" << maxHealth;
				m_HealthText->SetText(ss.str());
			}
			return (curHealth / (float)maxHealth);
		}
	}

	return 0.0f;
}
