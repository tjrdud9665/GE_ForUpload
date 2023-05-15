#pragma once

#include "XObject/XObject.h"

class EditorManager : public XObject
{
	friend class Engine;
	friend class Globals;

private:
	EditorManager();
	virtual ~EditorManager();

private:
	void Initialize();
	void BeginWindow();

	void Update(float DeltaSeconds);

private:
	bool m_EditorBegun;
};

