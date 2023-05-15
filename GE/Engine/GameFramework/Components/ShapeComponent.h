#pragma once
#include "PrimitiveComponent.h"





//Physics Simulation 을 사용하는 경우 RigidBody로 확장
//이외의 경우는 Ghost 를 이용하고 수동으로 Resolve (Movement Component 에서 확장)
//Capsule 을 사용하는 캐릭터 -> 피직스 시뮬을 끄고 사용

class ShapeComponent :public PrimitiveComponent
{
	CLASS(ShapeComponent, PrimitiveComponent)

protected:
	ShapeComponent();
	ShapeComponent(const ShapeComponent& Other);
	virtual ~ShapeComponent();

	virtual void PreUpdate(float DeltaSeconds) override;
	
	//Never ShadowCasting 
public:
	virtual bool IsCastShadow() const override
	{
		return false;
	}


};

