#include "EnginePCH.h"
#include "NavMeshBoundsVolume.h"
#include <Physics/PhysicsSystem.h>
#include <Physics/PhysicsDebugDraw.h>
#include <Physics/BulletPhysicsHelper.h>
#include <Navigation/NavigationSystem.h>

NavMeshBoundsVolume::NavMeshBoundsVolume()
{
	BoxSize = SVector3(200, 200, 200);

}

NavMeshBoundsVolume::NavMeshBoundsVolume(const NavMeshBoundsVolume& Other)
	:Super(Other)
	, BoxSize(Other.BoxSize)
{
	m_Root = FindComponent<SceneComponent>("Root");
	m_cachedNav = g_Engine->Get<NavigationSystem>();
}

NavMeshBoundsVolume::~NavMeshBoundsVolume()
{
	
}

void NavMeshBoundsVolume::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);	
	m_debugDraw = g_Engine->Get<PhysicsSystem>()->GetDebugDraw();

	m_Root = CreateComponent<SceneComponent>("Root");

	SetRootComponent(m_Root);	

}

void NavMeshBoundsVolume::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
	
	m_prevLocation = m_Root->GetWorldLocation();

	
}

void NavMeshBoundsVolume::PostUpdate(float DeltaSeconds)
{	
	Super::PostUpdate(DeltaSeconds);
	btTransform tr;
	tr.setIdentity();	
	tr.setOrigin(ToBtVector3(m_Root->GetWorldLocation()));
	tr.setRotation(ToBtQuaternion(SMathUtils::EulerToQuat(m_Root->GetWorldRotation())));

	

	m_scaledBox = GetRootComponent()->m_ComponentToWorld.Scale3D * BoxSize;

	if(m_debugDraw)
	{		
		m_debugDraw->drawBox(ToBtVector3(-m_scaledBox *0.5f), ToBtVector3(m_scaledBox * 0.5f), tr,ToBtVector3(SVector3(0,1,0)));
	}
	//////////////////////////////////////////////////////////////////////////
	//->GUI 에서 버튼뽑고 생성 하는 코드 제작..	
	

	if(m_prevLocation != m_Root->GetWorldLocation())
	{
		
		
		////GenNavMesh		
		//btVector3 bbmin = tr * ToBtVector3(-BoxSize * 0.5f);
		//btVector3 bbmax = tr * ToBtVector3(BoxSize * 0.5f);
		//		
		//m_max = ToVector3(bbmax);
		//m_min = ToVector3(bbmin);

		

	}		

	//m_cachedNav->DrawDebugNavMesh();

}

void NavMeshBoundsVolume::SetScene(class Scene* InOwningScene)
{
	Super::SetScene(InOwningScene);	
}

void NavMeshBoundsVolume::OnDestroy()
{
	Super::OnDestroy();
	

}
