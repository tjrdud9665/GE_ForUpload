#pragma once
#include "ShapeComponent.h"

//Sphere
//Box

//Capsule
 

//Movement

class BoxComponent :public ShapeComponent
{
	CLASS(BoxComponent,ShapeComponent)

protected:
	BoxComponent();
	BoxComponent(const BoxComponent& Other);
	virtual ~BoxComponent();


public:
	const SVector3& GetOriginBoxExtent() const;

protected:
	virtual void CreateShape() override;

	PROP(SVector3, BoxExtent, DetailsEdit, "Box")
	SVector3 BoxExtent;

};

