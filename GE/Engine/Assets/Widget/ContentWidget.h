#pragma once
#include "PanelWidget.h"

//�̱� ���ϵ带 ������ slot.


class ContentWidget :public PanelWidget
{
	CLASS(ContentWidget, PanelWidget)


protected:
	ContentWidget();
	ContentWidget(const ContentWidget& Other);
	virtual ~ContentWidget();

};

