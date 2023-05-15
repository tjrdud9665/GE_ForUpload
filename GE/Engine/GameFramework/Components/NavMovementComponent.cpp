#include "EnginePCH.h"
#include "NavMovementComponent.h"
#include <Navigation/NavigationSystem.h>
#include "GameFramework/Components/SceneComponent.h"

 NavMovementComponent::NavMovementComponent()
{

}

 NavMovementComponent::NavMovementComponent(const NavMovementComponent& Other)
	 :Super(Other)
	 , m_CachedNav(Other.m_CachedNav)
	 , m_AgentID(INDEX_NONE)
	 ,AgentHeight(Other.AgentHeight)
	 , AgentRadius(Other.AgentRadius)
 {

 }

 NavMovementComponent::~NavMovementComponent()
 {

 }

 void NavMovementComponent::Construct(const std::string& InName /*= ""*/)
 {
	 Super::Construct(InName);
	 m_CachedNav = g_Engine->Get<NavigationSystem>();
	 

 }

 void NavMovementComponent::BeginPlay()
 {
	 Super::BeginPlay(); 
	 
	 //if (m_CachedNav)
	 //{
		// m_AgentID = m_CachedNav->AddAgent(GetUpdateComponent()->GetWorldLocation());
	 //}
	 //PrimitiveComponent* prim = (PrimitiveComponent*)GetUpdateComponent();	 
	 //prim->BindOnBeginOverlap()
	 //m_CachedNav->SetAgentLocation(m_AgentID, GetUpdateComponent()->GetWorldLocation());
	 
 }

 void NavMovementComponent::Update(float DeltaSeconds)
 {
	 Super::Update(DeltaSeconds);

	 if (m_AgentID != INDEX_NONE)
	 {
		 //Find Ground Location ray to down
		 //SHitResult hit = m_cachedPhysics->rayTest(GetUpdateComponent()->GetWorldLocation(), SVector3::DownVector, 1000);

		 SVector3 loc = GetUpdateComponent()->GetWorldLocation();
		 m_CachedNav->GetAgentLocation(m_AgentID,OUT loc);
		 m_cachedPhysics->GetDebugDraw()->drawLine(ToBtVector3(GetUpdateComponent()->GetWorldLocation()), ToBtVector3(loc), btVector3(1, 0, 0));
		
	 }
	 
	 
	 
	
 }

 void NavMovementComponent::EndPlay()
 {
	 Super::EndPlay();
	 if (m_AgentID == INDEX_NONE)
	 {
		 //Find Ground Location ray to down
		 m_CachedNav->RemoveAgent(OUT m_AgentID);		 
	 }

 }

 float NavMovementComponent::GetMaxSpeed() const
 {
	 return 0.0f;
 }

 void NavMovementComponent::MoveToTargetLocation(const SVector3& targetLoc)
 {
	 if (m_CachedNav)
	 {
		 if(m_AgentID == INDEX_NONE)
		 {
			 //Find Ground Location ray to down
			 SHitResult hit = m_cachedPhysics->rayTest(GetUpdateComponent()->GetWorldLocation(), SVector3::DownVector, 1000);		 
			 m_AgentID = m_CachedNav->AddAgent(hit.point);
		 }
	 		 

		 m_CachedNav->SetAgentVelocity(m_AgentID, GetMaxSpeed());
		 m_CachedNav->SetAgentHeight(m_AgentID, AgentHeight);
		 m_CachedNav->SetAgentRadius(m_AgentID, AgentRadius);
		 m_CachedNav->SetAgentLocation(m_AgentID, GetUpdateComponent()->GetWorldLocation());
		 m_CachedNav->SetMoveTarget(m_AgentID,targetLoc);
		 
		 Destination = targetLoc;
		 m_NavFollowPath = true;
	 }
 }

 void NavMovementComponent::StopPathFollow()
 {
	 m_NavFollowPath = false;
 }

