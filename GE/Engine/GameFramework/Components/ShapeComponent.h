#pragma once
#include "PrimitiveComponent.h"





//Physics Simulation �� ����ϴ� ��� RigidBody�� Ȯ��
//�̿��� ���� Ghost �� �̿��ϰ� �������� Resolve (Movement Component ���� Ȯ��)
//Capsule �� ����ϴ� ĳ���� -> ������ �ù��� ���� ���

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

