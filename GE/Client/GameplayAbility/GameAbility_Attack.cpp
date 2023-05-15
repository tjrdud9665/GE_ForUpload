#include "PCH.h"
#include "GameAbility_Attack.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include "ObjectManager.h"
#include "../GameObject/HitBoxObject.h"
#include "GameplaySystem/GameplaySystem.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/GameObject/Character.h"
#include "GameFramework/Components/SkeletalMeshComponent.h"




GameAbility_Attack::GameAbility_Attack()
{

}

GameAbility_Attack::GameAbility_Attack(const GameAbility_Attack& Other)
	:Super(Other)
	, AttackSet(Other.AttackSet)
{
	size_t Size = Other.Attack.size();

	Attack.resize(Size);

	for (int i = 0; i < Size; ++i)
	{
		Attack[i] = Other.Attack[i];
	}
}

GameAbility_Attack::~GameAbility_Attack()
{

}

void GameAbility_Attack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{

	if (EventName == "Spawn")
	{
		ObjectManager* Obj = g_Engine->Get<ObjectManager>();

		HitBoxObject* spawnedObject = Obj->SpawnGameObject<HitBoxObject>("HitBox");

		SVector3 Pos = m_ObjectInfo.OwnerObject->GetWorldLocation();

		SVector3 Dir = m_ObjectInfo.OwnerObject->GetObjectForwardVector();

		SVector3 Rot = m_ObjectInfo.OwnerObject->GetWorldRotation();		

		spawnedObject->SetShooter(m_ObjectInfo.OwnerObject);			
		spawnedObject->SetColiisionProfileName("PlayerAttack");
		spawnedObject->SetEventName("Attack");	
		spawnedObject->SetObjectLocation(Pos + Dir * 500.f);
		spawnedObject->SetObjectRotation(SVector3(0.f, 0.f, Rot.y));
		spawnedObject->SetObjectScale(SVector3(100.f, 50.f, 50.f));

		//이곳에서 충돌체소환하면서 충돌체에 누가 공격자(슈터) 인지 등록해준다 .
	}

	if (EventName == "Attack")
	{
		// 공격에 성공할경우 이곳으로 이벤트 발생하고 , 타겟의 정보를 얻어와 해당 타겟의 어빌리티컴포넌트를 이펙트 처리 해준다 . 

		if (!EventData)
			return;

		IAbilitySystem* abilitySys = dynamic_cast<IAbilitySystem*>(EventData->Target);

		if (abilitySys)
		{

			AbilityComponent* targetComp = abilitySys->GetAbilityComponent();
			AbilityComponent* ShooterComp = m_ObjectInfo.OwnerComponent;

			//targetComp->ApplyGameplayEffectToTarget(AttackSet, targetComp);
		}

	
	}

	if (EventName == "Test")
	{
		AbilityComponent* srcComp = m_ObjectInfo.OwnerComponent;

		if (srcComp)
			srcComp->ApplyGameplayEffectToSelt(AttackSet);

	}
}

void GameAbility_Attack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	AbilityUtils::PlayAnimation(this, Attack[0], false, true, true);
}

void GameAbility_Attack::OnCancelAbility()
{

}

void GameAbility_Attack::OnEndAbility()
{
	m_ObjectInfo.OwnerAvatar->SetEnableRootMotion(false);
}

void GameAbility_Attack::InputPressed(const SGameplayAbilitySpec& InSpec)
{

}

void GameAbility_Attack::InputReleased(const SGameplayAbilitySpec& InSpec)
{

}
