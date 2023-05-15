#pragma once
#include "SceneComponent.h"
class TargetArmComponent :public SceneComponent
{
	CLASS(TargetArmComponent,SceneComponent)

protected:
	TargetArmComponent();
	TargetArmComponent(const TargetArmComponent& Other);
	virtual ~TargetArmComponent();


	virtual void BeginPlay() override;


	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

protected:
	PROP(SVector3, m_TargetOffset, DetailsEdit)
	SVector3	m_TargetOffset;

	PROP(float, m_TargetDistance, DetailsEdit)
	float		m_TargetDistance;

	PROP(TEnum<EAxis>, m_TargetDistanceAxis, DetailsEdit)
	TEnum<EAxis>	m_TargetDistanceAxis;


	PROP(float, m_TargetSpeed, DetailsEdit)
	float		m_TargetSpeed;

	SVector3	m_PrevPos;

};

