#pragma once

#include "XObject/XObject.h" 
/**
 * Scene 에서 생성해서 엔진에 부착합니다.
 * 각 Scene 별로 가지고있으며 Scene 이시작될떄 Scene 의 CameraManager가 엔진에부착되며 
 * 내려갈때 떄갑니다.
 */
class CameraManager :public XObject
{
	friend class Scene;
	friend class Globals;

private:
	CameraManager();
	virtual ~CameraManager();

private:
	TSharedPtr<class CameraComponent>	m_CurrentCamera;
	TSharedPtr<class CameraComponent>	m_UICamera;
	TSharedPtr<class CameraComponent>	m_DefaultCamera;

public:
	void SetDefaultCamera();

	class CameraComponent* GetCurrentCamera()	const
	{
		return m_CurrentCamera;
	}

	class CameraComponent* GetUICamera()	const
	{
		return m_UICamera;
	}

public:
	void SetCurrentCamera(class CameraComponent* Camera);
	void SetUICamera(class CameraComponent* Camera);

public:
	void BeginPlay();
	void EndPlay();
	bool Initialize();
	void PreUpdate(float DeltaSeconds);
	void Update(float DeltaSeconds);
	void PostUpdate(float DeltaSeconds);
	
};

