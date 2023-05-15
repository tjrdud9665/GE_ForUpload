#pragma once

#include "XObject/XObject.h"


class Editor : public XObject
{
	CLASS(Editor,XObject)

protected:
	Editor();
	virtual ~Editor();

public:
	virtual void ExpandViewsMenu() {}

	//Editor 에서 그리기 요청을 써야하는경우
	virtual void Render(){}
	virtual void PreRender(){}

};


