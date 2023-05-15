#include "EnginePCH.h"
#include "ContentWidget.h"


//ContentWidget Only One Child.

ContentWidget::ContentWidget()
{
	bCanHaveMultipleChildren = false;
}

ContentWidget::ContentWidget(const ContentWidget& Other)
	:Super(Other)
{
	bCanHaveMultipleChildren = false;
}

ContentWidget::~ContentWidget()
{

}
