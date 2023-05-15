#pragma once
#include "ShapeComponent.h"

class CapsuleComponent : public ShapeComponent
{
	CLASS(CapsuleComponent,ShapeComponent)

protected:
	CapsuleComponent();
	CapsuleComponent(const CapsuleComponent& Other);
	virtual ~CapsuleComponent();

public:
	virtual void BeginPlay() override;


	virtual void PreUpdate(float DeltaSeconds) override;
	
	float GetHalfHeight() const;

	float GetRadius() const;

protected:
	virtual void CreateShape() override;

	//virtual void SetupCollisionFlags() override;

private:
	PROP(float, Radius, DetailsEdit, "Shape")
	float	Radius;
	float	prevRadius;

	PROP(float, HalfHeight, DetailsEdit, "Shape")
	float HalfHeight;
	float prevHeight;
};

