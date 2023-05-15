#pragma once

#define ENTRYAPP(x,...) Application* EntryApplication(){return new x(__VA_ARGS__);}



struct SEngineInitialInfo
{
	wstring WndTitle;
	int32 WndWidth;
	int32 WndHeight;
	int32 dxWidth;
	int32 dxHeight;
	bool FullScreen;
	SEngineInitialInfo(const wstring& InTitle, int32 _WndWidth, int32 _WndHeight, int32 _dxWidth, int32 _dxHeight, bool _FullScreen)
		:WndTitle(InTitle), WndWidth(_WndWidth), WndHeight(_WndHeight), dxWidth(_dxWidth), dxHeight(_dxHeight), FullScreen(_FullScreen)
	{

	}


};

class Application
{
	friend class Engine;

public:
	Application(const wstring& InTitle, int32 WndWidth, int32 WndHeight, int32 dxWidth, int32 dxHeight, bool FullScreen);
	virtual ~Application();

	

	virtual void Initialize();

	//* 엔진의 첫 업데이트를 돌고 난 직후
	virtual void PostFirstUpdate(float DeltaSeconds);


	void _SetEngine(class Engine* InEngine);

	virtual void PreInitialize() {}

	SEngineInitialInfo GetInitInfo() const;

	virtual bool OnMessageHandle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual bool IsConsumeIMGUIMouseInput() const
	{
		return false;
	}

	virtual bool IsConsumeIMGUIKeyInput() const
	{
		return false;
	}

private:
	virtual void Update(float DeltaSeconds) { }


protected:
	class Engine* m_Engine;
	SEngineInitialInfo	m_InitInfo;

};

