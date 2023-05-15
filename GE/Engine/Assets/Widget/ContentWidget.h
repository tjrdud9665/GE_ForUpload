#pragma once
#include "PanelWidget.h"

//싱글 차일드를 가지는 slot.


class ContentWidget :public PanelWidget
{
	CLASS(ContentWidget, PanelWidget)


protected:
	ContentWidget();
	ContentWidget(const ContentWidget& Other);
	virtual ~ContentWidget();

};

