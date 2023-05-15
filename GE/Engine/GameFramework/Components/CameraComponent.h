#pragma once
#include "SceneComponent.h"
/*
* 카메라의 월드행렬의 역행렬은 곧 뷰 행렬이다.
*/



class CameraComponent :public SceneComponent
{
	CLASS(CameraComponent,SceneComponent)
	friend class GameObject;
	friend class CameraManager;

	friend class GuizmoWidget;

protected:
	CameraComponent();
	CameraComponent(const CameraComponent& Other);
	virtual ~CameraComponent();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void Render(class Renderer* pRenderer) override;


	void unproject(const D3D11_VIEWPORT& viewport, float x, float y, float depth, SVector3* dst) const;

	SVector3 ScreenToWorldCoordinates(const SVector2& position_screen, const float z) const;

public:
	SHitResult pickRay(const D3D11_VIEWPORT& viewport, float x, float y, SVector3& OutRayOrigin , SVector3& OutRayDireciton , float MaxDistance = 9999999.0f) const;

public:
	void SetProjectionType(EProjectionType InProjectionType);

	void SetViewDistance(float Dist);

	const Matrix& GetViewMatrix() const
	{
		return m_matView;
	}

	const Matrix& GetProjMatrix() const
	{
		return m_matProj;
	}

	float GetViewDistance() const
	{
		return ViewDistance;
	}

	float GetFOV() const
	{
		return FieldOfView;
	}
	float GetAsepctRatio() const
	{
		return m_AspectRatio;
	}
	virtual void SetTransformFromMatrix(const Matrix& InMatrix) override;


private:
	PROP(float, ViewDistance, DetailsEdit, "Camera")
	float ViewDistance;

	PROP(float, FieldOfView, DetailsEdit,"Camera")
	float FieldOfView;

	//PROP()
	EProjectionType		ProjectionType;

	EProjectionType		PrevProjectionType;

protected:

	Matrix				m_matView;
	Matrix				m_matProj;

	float				m_AspectRatio;

};

