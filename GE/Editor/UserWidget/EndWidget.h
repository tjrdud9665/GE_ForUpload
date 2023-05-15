#pragma once
#include "Assets\Widget\UserWidget.h"
class EndWidget : public UserWidget
{

	CLASS(EndWidget, UserWidget)

protected:
	EndWidget();
	EndWidget(const EndWidget& Other);
	virtual~EndWidget();

	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;


private:
	void OnEndButtonPressed();


private:
	class CanvasPanel* m_RootPanel;

	class SizeBox* m_EndSceneSizeBox;
	class ButtonWidget* m_EndSceneButton;

};

