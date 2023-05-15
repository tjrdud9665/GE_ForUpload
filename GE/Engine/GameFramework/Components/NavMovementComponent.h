#pragma once
#include "MovementComponent.h"

//Agent 로 Obstacle 갱신
//NavAgent를 가지고있는

class NavMovementComponent :public MovementComponent
{
	CLASS(NavMovementComponent, MovementComponent)

	friend class NavigationSystem;
	friend struct SRecastZone;

protected:
	NavMovementComponent();
	NavMovementComponent(const NavMovementComponent& Other);
	virtual ~NavMovementComponent();


	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;

	virtual void Update(float DeltaSeconds) override;

	virtual void EndPlay() override;
public:
	virtual float GetMaxSpeed() const;

	virtual void MoveToTargetLocation(const SVector3& targetLoc);

	virtual void StopPathFollow();

protected:
	class NavigationSystem* m_CachedNav;
	int32		m_AgentID;

	PROP(float, AgentRadius, DetailsEdit, "NavAgent")
	float AgentRadius = 34;

	PROP(float, AgentHeight, DetailsEdit, "NavAgent")
	float AgentHeight = 10;


	SVector3	Destination;
	bool m_NavFollowPath;


};

