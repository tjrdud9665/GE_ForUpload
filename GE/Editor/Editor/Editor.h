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

	//Editor ���� �׸��� ��û�� ����ϴ°��
	virtual void Render(){}
	virtual void PreRender(){}

};


