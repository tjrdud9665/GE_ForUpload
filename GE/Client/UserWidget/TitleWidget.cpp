#include "PCH.h"
#include "TitleWidget.h"
#include <Assets/Widget/CanvasPanel.h>
#include <Assets/Widget/SizeBox.h>
#include <Assets/Widget/ButtonWidget.h>
#include <Assets/Widget/WidgetTree.h>
#include <SceneManager.h>
#include <Path/PathManager.h>
#include <Assets/Scene/Scene.h>
#include <AssetManager.h>

TitleWidget::TitleWidget()
{

}

TitleWidget::TitleWidget(const TitleWidget& Other)
	:Super(Other)
{
	//class CanvasPanel* m_RootPanel;

	//class SizeBox* m_StartSizeBox;
	//class ButtonWidget* m_StartButton;

	//class SizeBox* m_EndSizeBox;
	//class ButtonWidget* m_EndButton;
	if (Other.IsConstructed())
	{
		

		m_RootPanel = m_WidgetTree->FindWidget<CanvasPanel>("RootPanel_1");
		m_StartSizeBox = m_WidgetTree->FindWidget<SizeBox>("StartSize_1");
		m_StartButton = m_WidgetTree->FindWidget<ButtonWidget>("StartButton_1");
		m_EndSizeBox = m_WidgetTree->FindWidget<SizeBox>("EndSize_1");
		m_EndButton = m_WidgetTree->FindWidget<ButtonWidget>("EndButton_1");
	}
}

TitleWidget::~TitleWidget()
{

}

void TitleWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
	m_RootPanel = m_WidgetTree->ConstructWidget<CanvasPanel>(CanvasPanel::StaticClass(), "RootPanel");
	m_StartSizeBox = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "StartSize");
	m_StartButton = m_WidgetTree->ConstructWidget<ButtonWidget>(ButtonWidget::StaticClass(), "StartButton");
	m_EndSizeBox = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "EndSize");
	m_EndButton = m_WidgetTree->ConstructWidget<ButtonWidget>(ButtonWidget::StaticClass(), "EndButton");

	m_WidgetTree->SetRootWidget(m_RootPanel);
	m_RootPanel->AddChild(m_StartSizeBox);
	m_StartSizeBox->AddChild(m_StartButton);

	m_RootPanel->AddChild(m_EndSizeBox);
	m_EndSizeBox->AddChild(m_EndButton);
}

void TitleWidget::BeginPlay()
{
	Super::BeginPlay();

	m_StartButton->BindOnPressed(this, &TitleWidget::OnStartButtonPressed);
	m_EndButton->BindOnPressed(this, &TitleWidget::OnEndButtonPressed);
}

void TitleWidget::OnStartButtonPressed()
{
	const SPathInfo* pathInfo = g_Engine->Get<PathManager>()->FindPath(SCENE_PATH);
	std::string scenePath = pathInfo->aPath + "Main.meta";

	Scene* scene = (Scene*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(scenePath), Scene::StaticClass());
	g_Engine->Get<SceneManager>()->SetAutoPlay(true);
	g_Engine->Get<SceneManager>()->CreateNextScene(scene, true);
}

void TitleWidget::OnEndButtonPressed()
{
	g_Engine->ShutDown();
}
