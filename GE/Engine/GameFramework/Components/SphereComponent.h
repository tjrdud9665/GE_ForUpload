#pragma once
#include "ShapeComponent.h"

class SphereComponent :public ShapeComponent
{
	CLASS(SphereComponent, ShapeComponent)

protected:
	SphereComponent();
	SphereComponent(const SphereComponent& Other);
	virtual ~SphereComponent();

	float GetOriginRadius() const;
protected:
	virtual void CreateShape() override;

	PROP(float, Radius, DetailsEdit, "Sphere")
	float Radius;
};

