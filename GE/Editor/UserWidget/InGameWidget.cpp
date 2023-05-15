#include "PCH.h"
#include "InGameWidget.h"
#include <Assets/Widget/CanvasPanel.h>
#include <Assets/Widget/SizeBox.h>
#include <Assets/Widget/Overlay.h>
#include <Assets/Widget/WidgetTree.h>
#include <Assets/Widget/TextBlock.h>
#include <Assets/Widget/ProgressBar.h>
#include <GameFramework/GameObject/PlayerCharacter.h>
#include "GameObject/Player.h"
#include <Assets/Scene/Scene.h>
#include <Assets/SceneInfo/SceneInfo.h>
#include <GameplayAbility/CharacterAttributeSet.h>

InGameWidget::InGameWidget()
{

}

InGameWidget::InGameWidget(const InGameWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{

		m_RootPanel = m_WidgetTree->FindWidget<CanvasPanel>("RootPanel_1");
		m_HeathBarSize = m_WidgetTree->FindWidget<SizeBox>("HeathBarSize_1");
		m_ProgressOverlay = m_WidgetTree->FindWidget<Overlay>("ProgressOverlay_1");
		m_HealthBar = m_WidgetTree->FindWidget<ProgressBar>("HealthBar_1");
		m_HealthText = m_WidgetTree->FindWidget<TextBlock>("HealthText_1");

		m_Skill1SlotSize = m_WidgetTree->FindWidget<SizeBox>("Skill1SlotSize_1");
		m_Skill1Overlay = m_WidgetTree->FindWidget<Overlay>("Skill1Overlay_1");
		m_Skill1Bar = m_WidgetTree->FindWidget<ProgressBar>("Skill1Bar_1");

		m_Skill2SlotSize = m_WidgetTree->FindWidget<SizeBox>("Skill2SlotSize_1");
		m_Skill2Overlay = m_WidgetTree->FindWidget<Overlay>("Skill2Overlay_1");
		m_Skill2Bar = m_WidgetTree->FindWidget<ProgressBar>("Skill2Bar_1");

		m_Skill3SlotSize = m_WidgetTree->FindWidget<SizeBox>("Skill3SlotSize_1");
		m_Skill3Overlay = m_WidgetTree->FindWidget<Overlay>("Skill3Overlay_1");
		m_Skill3Bar = m_WidgetTree->FindWidget<ProgressBar>("Skill3Bar_1");

		m_Skill4SlotSize = m_WidgetTree->FindWidget<SizeBox>("Skill4SlotSize_1");
		m_Skill4Overlay = m_WidgetTree->FindWidget<Overlay>("Skill4Overlay_1");
		m_Skill4Bar = m_WidgetTree->FindWidget<ProgressBar>("Skill4Bar_1");
	}
}

InGameWidget::~InGameWidget()
{

}

void InGameWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_RootPanel = m_WidgetTree->ConstructWidget<CanvasPanel>(CanvasPanel::StaticClass(), "RootPanel");
	m_HeathBarSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "HeathBarSize");
	m_ProgressOverlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "ProgressOverlay");
	m_HealthBar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "HealthBar");
	m_HealthText = m_WidgetTree->ConstructWidget<TextBlock>(TextBlock::StaticClass(), "HealthText");

	m_Skill1SlotSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "Skill1SlotSize");
	m_Skill1Overlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "Skill1Overlay");
	m_Skill1Bar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "Skill1Bar");

	m_Skill2SlotSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "Skill2SlotSize");
	m_Skill2Overlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "Skill2Overlay");
	m_Skill2Bar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "Skill2Bar");

	m_Skill3SlotSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "Skill3SlotSize");
	m_Skill3Overlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "Skill3Overlay");
	m_Skill3Bar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "Skill3Bar");

	m_Skill4SlotSize = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "Skill4SlotSize");
	m_Skill4Overlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "Skill4Overlay");
	m_Skill4Bar = m_WidgetTree->ConstructWidget<ProgressBar>(ProgressBar::StaticClass(), "Skill4Bar");

	m_WidgetTree->SetRootWidget(m_RootPanel);
	m_RootPanel->AddChild(m_HeathBarSize);
	m_HeathBarSize->AddChild(m_ProgressOverlay);
	m_ProgressOverlay->AddChild(m_HealthBar);
	m_ProgressOverlay->AddChild(m_HealthText);


	m_RootPanel->AddChild(m_Skill1SlotSize);
	m_Skill1SlotSize->AddChild(m_Skill1Overlay);
	m_Skill1Overlay->AddChild(m_Skill1Bar);

	m_RootPanel->AddChild(m_Skill2SlotSize);
	m_Skill2SlotSize->AddChild(m_Skill2Overlay);
	m_Skill2Overlay->AddChild(m_Skill2Bar);

	m_RootPanel->AddChild(m_Skill3SlotSize);
	m_Skill3SlotSize->AddChild(m_Skill3Overlay);
	m_Skill3Overlay->AddChild(m_Skill3Bar);

	m_RootPanel->AddChild(m_Skill4SlotSize);
	m_Skill4SlotSize->AddChild(m_Skill4Overlay);
	m_Skill4Overlay->AddChild(m_Skill4Bar);



}

void InGameWidget::BeginPlay()
{
	Super::BeginPlay();

	m_HealthBar->BindPecent(std::bind(&InGameWidget::GetHealth, this));
	m_Skill1Bar->BindPecent(std::bind(&InGameWidget::GetSkill1Percent, this));
	m_Skill2Bar->BindPecent(std::bind(&InGameWidget::GetSkill2Percent, this));
	m_Skill3Bar->BindPecent(std::bind(&InGameWidget::GetSkill3Percent, this));
	m_Skill4Bar->BindPecent(std::bind(&InGameWidget::GetSkill4Percent, this));

}

float InGameWidget::GetHealth()
{
	if (m_OwningScene)
	{
		SceneInfo* scnInfo = m_OwningScene->GetSceneInfo();
		if (scnInfo)
		{
			Character* pc = scnInfo->GetPlayerCharacter();
			if (pc)
			{
				CharacterAttributeSet* targetAttSet = pc->GetAttributeSetAs<CharacterAttributeSet>();


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

	}

	return 0.0f;
}

float InGameWidget::GetSkill1Percent()
{
	if (m_OwningScene)
	{
		SceneInfo* scnInfo = m_OwningScene->GetSceneInfo();
		if (scnInfo)
		{
			Player* pc = dynamic_cast<Player*>(scnInfo->GetPlayerCharacter());

			if (pc)
			{
				GameplayAbility* AbilityInst = pc->GetAbilityComponent()->GetAbilityInsance(pc->GetSkill1());

				if (AbilityInst)
				{
					uint64 CoolID = AbilityInst->GetCoolTimeID();

					float remainingTime = pc->GetAbilityComponent()->GetEffectRemainingTime(CoolID);
					//AbilityInst->Cool;

					return remainingTime / 4.f;
				}
			}

		}

	}

	return 0.0f;
}

float InGameWidget::GetSkill2Percent()
{
	if (m_OwningScene)
	{
		SceneInfo* scnInfo = m_OwningScene->GetSceneInfo();
		if (scnInfo)
		{
			Player* pc = dynamic_cast<Player*>(scnInfo->GetPlayerCharacter());

			if (pc)
			{
				GameplayAbility* AbilityInst = pc->GetAbilityComponent()->GetAbilityInsance(pc->GetSkill2());

				if (AbilityInst)
				{
					uint64 CoolID = AbilityInst->GetCoolTimeID();

					float remainingTime = pc->GetAbilityComponent()->GetEffectRemainingTime(CoolID);

					return remainingTime / 7.f;
				}
			}

		}

	}

	return 0.0f;
}

float InGameWidget::GetSkill3Percent()
{
	if (m_OwningScene)
	{
		SceneInfo* scnInfo = m_OwningScene->GetSceneInfo();
		if (scnInfo)
		{
			Player* pc = dynamic_cast<Player*>(scnInfo->GetPlayerCharacter());

			if (pc)
			{
				GameplayAbility* AbilityInst = pc->GetAbilityComponent()->GetAbilityInsance(pc->GetSkill3());

				if (AbilityInst)
				{
					uint64 CoolID = AbilityInst->GetCoolTimeID();

					float remainingTime = pc->GetAbilityComponent()->GetEffectRemainingTime(CoolID);

					return remainingTime / 15.f;
				}
			}

		}

	}

	return 0.0f;
}

float InGameWidget::GetSkill4Percent()
{
	if (m_OwningScene)
	{
		SceneInfo* scnInfo = m_OwningScene->GetSceneInfo();
		if (scnInfo)
		{
			Player* pc = dynamic_cast<Player*>(scnInfo->GetPlayerCharacter());

			if (pc)
			{
				GameplayAbility* AbilityInst = pc->GetAbilityComponent()->GetAbilityInsance(pc->GetSkill4());

				if (AbilityInst)
				{
					uint64 CoolID = AbilityInst->GetCoolTimeID();

					float remainingTime = pc->GetAbilityComponent()->GetEffectRemainingTime(CoolID);

					return remainingTime / 10.f;
				}
			}

		}

	}

	return 0.0f;
}
