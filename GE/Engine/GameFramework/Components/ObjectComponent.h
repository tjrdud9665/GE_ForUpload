#pragma once
#include "ComponentBase.h"

class ObjectComponent :public ComponentBase
{
	CLASS(ObjectComponent, ComponentBase)
	
protected:
	ObjectComponent();
	virtual ~ObjectComponent();

};

