#pragma once
#include <Assets/Widget/UserWidget.h>

class InteractionWidget : public UserWidget
{
	CLASS(InteractionWidget, UserWidget)

protected:
	InteractionWidget();
	InteractionWidget(const InteractionWidget& Other);
	virtual~InteractionWidget();

	virtual void Construct(const std::string& InName = "") override;

private:
	class CanvasPanel* m_RootPanel;
};

