#pragma once

#include "Application.h"
#include "Entry.h"
#include "Timer/TimerHandle.h"
//#include <Renderer/RenderCommands.h>


class EditorApp :public Application
{
public:
	EditorApp(const wstring& InTitle, int32 WndWidth, int32 WndHeight, int32 dxWidth, int32 dxHeight, bool FullScreen);
	virtual ~EditorApp();

	virtual void PreInitialize() override;
	virtual void Initialize() override;
	

	virtual void Update(float DeltaSeconds) override;

	
	virtual void PostFirstUpdate(float DeltaSeconds) override;

	virtual bool OnMessageHandle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual bool IsConsumeIMGUIMouseInput() const override;

	virtual bool IsConsumeIMGUIKeyInput() const override;

private:
	void FPSTick();

	void MLBTest();



private:
	class EditorManager* m_EditorManager;

	TimerHandle			m_AppTimer;
	uint64				m_FPSTimerID;

	uint32				m_FrameCounter;

	//std::vector<SDrawLineCommand>	m_test;

			

};
ENTRYAPP(EditorApp, L"Editor", 1920, 1080, 1920, 1080, false)

//ENTRYAPP(EditorApp, L"Editor", 1600, 900, 1600, 900, false)

