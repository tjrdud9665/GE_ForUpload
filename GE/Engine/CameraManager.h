#pragma once

#include "XObject/XObject.h" 
/**
 * Scene ���� �����ؼ� ������ �����մϴ�.
 * �� Scene ���� ������������ Scene �̽��۵ɋ� Scene �� CameraManager�� �����������Ǹ� 
 * �������� �����ϴ�.
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

