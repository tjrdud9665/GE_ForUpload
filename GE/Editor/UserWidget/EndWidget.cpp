#include "PCH.h"
#include "EndWidget.h"
#include <Assets/Widget/CanvasPanel.h>
#include <Assets/Widget/SizeBox.h>
#include <Assets/Widget/ButtonWidget.h>
#include <Assets/Widget/WidgetTree.h>
#include <SceneManager.h>
#include <Path/PathManager.h>
#include <Assets/Scene/Scene.h>
#include <AssetManager.h>

EndWidget::EndWidget()
{

}

EndWidget::EndWidget(const EndWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{
		m_RootPanel = m_WidgetTree->FindWidget<CanvasPanel>("RootPanel_1");
		m_EndSceneSizeBox = m_WidgetTree->FindWidget<SizeBox>("EndSceneSize_1");
		m_EndSceneButton = m_WidgetTree->FindWidget<ButtonWidget>("EndSceneButton_1");	
	}
}

EndWidget::~EndWidget()
{

}

void EndWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	m_RootPanel = m_WidgetTree->ConstructWidget<CanvasPanel>(CanvasPanel::StaticClass(), "RootPanel");
	m_EndSceneSizeBox = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "EndSceneSize");
	m_EndSceneButton = m_WidgetTree->ConstructWidget<ButtonWidget>(ButtonWidget::StaticClass(), "EndSceneButton");


	m_WidgetTree->SetRootWidget(m_RootPanel);
	m_RootPanel->AddChild(m_EndSceneSizeBox);
	m_EndSceneSizeBox->AddChild(m_EndSceneButton);

}

void EndWidget::BeginPlay()
{
	Super::BeginPlay();

	m_EndSceneButton->BindOnPressed(this, &EndWidget::OnEndButtonPressed);
}

void EndWidget::OnEndButtonPressed()
{
	g_Engine->ShutDown();
}
