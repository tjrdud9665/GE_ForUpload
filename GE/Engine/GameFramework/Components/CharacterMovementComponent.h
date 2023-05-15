#pragma once
#include "NavMovementComponent.h"


class btPlayerAction : public btKinematicCharacterController
{
public:
	btPlayerAction(class btPairCachingGhostObject* ghostObject, btConvexShape* convexShape, btScalar stepHeight, const btVector3& up);
	virtual bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1) override;
};


class CharacterMovementComponent :public NavMovementComponent
{
	CLASS(CharacterMovementComponent,NavMovementComponent)

protected:
	CharacterMovementComponent();
	CharacterMovementComponent(const CharacterMovementComponent& Other);
	virtual ~CharacterMovementComponent();

	virtual btActionInterface* InitActionInterface() override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;
	

public:
	// -> CharacterMovement ∑Œ ¿Ãµø
	virtual float GetMaxSpeed() const override;

	void SetMaxSpeed(float InSpeed);

	virtual void AddInputVector(const SVector3& WorldVector);

	const SVector3& ConsumeMovementInputVector();

	const SVector3& GetLastControllInputVector() const;

	float GetCurrentVelocity() ;

	virtual void ApplyToWorld() override;

	void ApplyRootMotionAcc(const SVector3& RootAcc);

	bool IsPathFollowing() const;




protected:
	float stepHeight = 30.0f;

	PROP(float, MaxWalkSpeed, DetailsEdit, "WalkSpeed")
		float MaxWalkSpeed;

	SVector3	m_ControllInputVector;

	SVector3	m_LastControllInputVector;

	SVector3	m_LastInputDirection;

	PROP(float, RotationRate, DetailsEdit, "Rotation")
	float		RotationRate;


	PROP(std::string, CantMoveTag, DetailsEdit, "MovementState")
	std::string CantMoveTag;
	//SVector3	m_ExternalVelocity;

	
};

