#pragma once


class EngineWindow : public XObject
{
	//CLASS(EngineWindow, XObject)

	friend class Engine;
	friend class Globals;
private:
	EngineWindow();
	~EngineWindow();

protected:
	static			LRESULT CALLBACK	SetupMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static			LRESULT	CALLBACK	AssignMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual			LRESULT				MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void	Initialize();

public:
	void	Initialize(const TCHAR* Title, uint32 Width, uint32 Height);


	SIZE	Size()const { return m_size; }
	HWND	Handle()const { return m_handle; }
	bool	IsWindow() const { return !m_bFullScreen; }



	void SetTitle(const wstring& title);




private:
	void RegisterWindowClass();
	


private:
	SIZE				m_size;
	bool				m_bFullScreen;
	HWND				m_handle;
	wstring				m_title;
	
};

