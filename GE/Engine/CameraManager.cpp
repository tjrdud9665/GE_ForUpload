#include "EnginePCH.h"
#include "CameraManager.h"
#include "GameFramework/Components/CameraComponent.h"

CameraManager::CameraManager()
{

}

CameraManager::~CameraManager()
{
	
}

void CameraManager::SetDefaultCamera()
{
	SetCurrentCamera(m_DefaultCamera);
}

void CameraManager::SetCurrentCamera(class CameraComponent* Camera)
{	
	m_CurrentCamera = Camera;	
}

void CameraManager::SetUICamera(class CameraComponent* Camera)
{
	m_UICamera = Camera;
}

void CameraManager::BeginPlay()
{

}

void CameraManager::EndPlay()
{

}

bool CameraManager::Initialize()
{
	m_DefaultCamera = m_CurrentCamera = Globals::CreateX<CameraComponent>(this);
	m_CurrentCamera->SetProjectionType(EProjectionType::Perspective);

	m_UICamera = Globals::CreateX<CameraComponent>(this);
	m_UICamera->SetProjectionType(EProjectionType::Orthographic);
	m_UICamera->SetViewDistance(1000.0f);
	

	return true;
}

void CameraManager::PreUpdate(float DeltaSeconds)
{
	if (m_CurrentCamera->GetOwner() == this)
	{
		m_CurrentCamera->PreUpdate(DeltaSeconds);
	}
}

void CameraManager::Update(float DeltaSeconds)
{
	if (m_CurrentCamera->GetOwner() == this)
	{
		m_CurrentCamera->Update(DeltaSeconds);
	}
}

void CameraManager::PostUpdate(float DeltaSeconds)
{
	if (m_CurrentCamera->GetOwner() == this)
	{
		m_CurrentCamera->PostUpdate(DeltaSeconds);
	}
}
