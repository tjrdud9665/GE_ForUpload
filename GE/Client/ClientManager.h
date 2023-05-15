#pragma once
#include "XObject/XObject.h"

//Ŭ���̾�Ʈ ������ ���� �Դϴ�.


class ClientManager :public XObject
{
	friend class ClientApp;	
	friend class Globals;
private:
	ClientManager();
	virtual ~ClientManager();

private:
	void Initialize();

	void Render();
	void PreRender();

private:
	FShaderID		m_FXAAShader;
	class Renderer* m_cachedRenderer;
	class SceneManager* m_cachedSceneManager;
	class RenderManager* m_cachedRenderManager;


};

