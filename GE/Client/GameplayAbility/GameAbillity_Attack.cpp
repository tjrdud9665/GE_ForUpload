#include "PCH.h"
#include "GameAbillity_Attack.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include "ObjectManager.h"
#include "GameFramework/GameObject/HitBoxObject.h"
#include "GameplaySystem/GameplaySystem.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/GameObject/Character.h"




GameAbillity_Attack::GameAbillity_Attack()
{

}

GameAbillity_Attack::GameAbillity_Attack(const GameAbillity_Attack& Other)
	:Super(Other)
	, BuffTest(Other.BuffTest)
{
	size_t Size = Other.Attack.size();

	Attack.resize(Size);

	for (int i = 0; i < Size; ++i)
	{
		Attack[i] = Other.Attack[i];
	}
}

GameAbillity_Attack::~GameAbillity_Attack()
{

}

void GameAbillity_Attack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{

	if (EventName == "Spawn")
	{
		ObjectManager* Obj = g_Engine->Get<ObjectManager>();

		HitBoxObject* spawnedObject = Obj->SpawnGameObject<HitBoxObject>("HitBox");

		SVector3 Pos = m_ObjectInfo.OwnerObject->GetWorldLocation();

		SVector3 Dir = m_ObjectInfo.OwnerObject->GetObjectForwardVector();

		SVector3 Rot = m_ObjectInfo.OwnerObject->GetWorldRotation();		

		spawnedObject->SetShooter(m_ObjectInfo.OwnerObject);			
		spawnedObject->SetColiisionProfileName("Ignore");
		spawnedObject->SetEventName("Attack");	
		spawnedObject->SetObjectLocation(Pos + Dir * 150.f);
		spawnedObject->SetObjectRotation(SVector3(0.f, 0.f, Rot.y));


		spawnedObject->SetObjectScale(SVector3(100.f, 50.f, 50.f));

		//�̰����� �浹ü��ȯ�ϸ鼭 �浹ü�� ���� ������(����) ���� ������ش� .
	}

	if (EventName == "Attack")
	{
		// ���ݿ� �����Ұ�� �̰����� �̺�Ʈ �߻��ϰ� , Ÿ���� ������ ���� �ش� Ÿ���� �����Ƽ������Ʈ�� ����Ʈ ó�� ���ش� . 

		if (!EventData)
			return;

		IAbilitySystem* abilitySys = dynamic_cast<IAbilitySystem*>(EventData->Target);

		if (abilitySys)
		{

			AbilityComponent* targetComp = abilitySys->GetAbilityComponent();
			AbilityComponent* ShooterComp = m_ObjectInfo.OwnerComponent;

			targetComp->ApplyGameplayEffectToTarget(BuffTest, targetComp);
		}

	
	}

	if (EventName == "Buff")
	{
		AbilityComponent* srcComp = m_ObjectInfo.OwnerComponent;

		if (srcComp)
			srcComp->ApplyGameplayEffectToSelt(BuffTest);

	}



}

void GameAbillity_Attack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	AbilityUtils::PlayAnimation(this, Attack[0], false, false,true);
}

void GameAbillity_Attack::OnCancelAbility()
{

}

void GameAbillity_Attack::OnEndAbility()
{

}

void GameAbillity_Attack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	int i = 0;
}

void GameAbillity_Attack::InputReleased(const SGameplayAbilitySpec& InSpec)
{
	int i = 0;
}
