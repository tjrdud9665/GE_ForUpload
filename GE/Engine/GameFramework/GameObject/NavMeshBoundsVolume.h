#pragma once
#include "GameObject.h"
//#include "LinearMath/btVector3.h"

class NavMeshBoundsVolume :public GameObject
{
	CLASS(NavMeshBoundsVolume, GameObject)

	friend class NavigationSystem;
	friend struct SRecastZone;

protected:
	NavMeshBoundsVolume();
	NavMeshBoundsVolume(const NavMeshBoundsVolume& Other);
	virtual ~NavMeshBoundsVolume();
	virtual void Construct(const std::string& InName /* = "" */) override;
	
	virtual void PreUpdate(float DeltaSeconds)					override;

	virtual void PostUpdate(float DeltaSeconds)					override;

	virtual void SetScene(class Scene* InOwningScene)			override;

	virtual void OnDestroy()									override;


public:



private:
	class SceneComponent* m_Root;
	class PhysicsDebugDraw* m_debugDraw;	

	PROP(SVector3,BoxSize, DetailsEdit ,"Box")
	SVector3			BoxSize;

	SVector3			m_scaledBox;

	SVector3			m_min;
	SVector3			m_max;

	SVector3			m_prevLocation;

	class NavigationSystem* m_cachedNav;

	PROP(float, CellSize, DetailsEdit, "NavSettings")
	float CellSize = 19.0f;

	PROP(float, CellHeight, DetailsEdit, "NavSettings")
	float CellHeight = 10.0f;

	PROP(float, WalkableSlope, DetailsEdit, "NavSettings")
	float WalkableSlope = 120.0f;
	
	PROP(float, AgentHeight, DetailsEdit, "NavSettings")
	float AgentHeight = 10.0f;

	PROP(float, MaxClimb, DetailsEdit, "NavSettings")
	float MaxClimb = 0.3f;

	PROP(float, AgentRadius, DetailsEdit, "NavSettings")
	float AgentRadius= 0.3f;

};

