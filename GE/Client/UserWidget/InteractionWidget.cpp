#include "PCH.h"
#include "InteractionWidget.h"
#include <Assets/Widget/WidgetTree.h>
#include <Assets/Widget/CanvasPanel.h>

InteractionWidget::InteractionWidget()
{
}

InteractionWidget::InteractionWidget(const InteractionWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{

		m_RootPanel = m_WidgetTree->FindWidget<CanvasPanel>("RootPanel_1");
	}
}

InteractionWidget::~InteractionWidget()
{
}

void InteractionWidget::Construct(const std::string& InName)
{
	Super::Construct(InName);

	m_RootPanel = m_WidgetTree->ConstructWidget<CanvasPanel>(CanvasPanel::StaticClass(), "RootPanel");





	m_WidgetTree->SetRootWidget(m_RootPanel);
}
