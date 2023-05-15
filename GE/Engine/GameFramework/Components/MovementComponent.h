#pragma once
#include "ObjectComponent.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"


//btActionInterface 상속을 통해서 캐릭터 이외의 투사체와 같은 물체의 이동의 동작을 정의

//btKinematicCharacterController 는 라이브러리 샘플이며 캐릭터용도로 사용



//Upto CharacterMovement
class MovementComponent :public ObjectComponent
{
	CLASS(MovementComponent, ObjectComponent)

protected:
	MovementComponent();
	MovementComponent(const MovementComponent& Other);
	virtual ~MovementComponent();

protected:
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void EndPlay() override;

	virtual void BeginPlay() override;	

	virtual void PostUpdate(float DeltaSeconds) override;

public:
	class SceneComponent* GetUpdateComponent() const;

	virtual void SetUpdateComponent(class SceneComponent* UpdateTarget);

	virtual void SetOwner(XObject* InOwner) override;

	//<-

	virtual void ApplyToWorld();


private:
	virtual class btActionInterface* InitActionInterface();

public:
	class btActionInterface* GetActionInterface() const;

	template<typename ActionObject, typename = std::enable_if_t<std::is_base_of<btActionInterface, ActionObject>::value>>
	ActionObject* GetActionInterfaceAs()
	{
		return (ActionObject*)m_btActionController;
	}


protected:
	TSharedPtr<class SceneComponent> m_UpdateComponent;
	
	class btActionInterface* m_btActionController;

	class PhysicsSystem* m_cachedPhysics;


	PROP(SVector3,Velocity,DetailsEdit,"Movement")
	SVector3		Velocity;
};


