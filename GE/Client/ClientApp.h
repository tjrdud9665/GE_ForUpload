#pragma once
#include "Entry.h"
#include <Application.h>

class ClientApp :public Application
{
public:
	ClientApp(const wstring& InTitle, int32 WndWidth, int32 WndHeight, int32 dxWidth, int32 dxHeight, bool FullScreen);
	virtual ~ClientApp();

	virtual void PreInitialize() override;
	virtual void Initialize() override;


	virtual void Update(float DeltaSeconds) override;

private:
	class ClientManager* m_ClientManager;
	
};

ENTRYAPP(ClientApp, L"Client", 1920, 1080, 1920, 1080, false)
