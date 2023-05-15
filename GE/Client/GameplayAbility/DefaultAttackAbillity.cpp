#include "PCH.h"
#include "DefaultAttackAbillity.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/GameObject/GameObject.h>
#include "ObjectManager.h"
#include "GameFramework/GameObject/ProjectileObject.h"

DefaultAttackAbillity::DefaultAttackAbillity()
{

}

DefaultAttackAbillity::DefaultAttackAbillity(const DefaultAttackAbillity& Other)
	:Super(Other)
	
{
	size_t Size = Other.DefaultAttack.size();

	DefaultAttack.resize(Size);

	for (int i = 0; i < Size; ++i)
	{
		DefaultAttack[i] = Other.DefaultAttack[i];
	}
}

DefaultAttackAbillity::~DefaultAttackAbillity()
{

}

void DefaultAttackAbillity::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (EventName == "Test")
	{
		if (m_CombeEnable)
		{
			++m_ComboCount;

			AbilityUtils::PlayAnimation(this, DefaultAttack[m_ComboCount], false, false);
			m_CombeEnable = false;
		}
	}

	if (EventName == "Spawn")
	{		
		ObjectManager* Obj = g_Engine->Get<ObjectManager>();

		ProjectileObject* spawnedObject = Obj->SpawnGameObject<ProjectileObject>("TestBox");

		SVector3 Pos = m_ObjectInfo.OwnerObject->GetWorldLocation();		

		spawnedObject->SetObjectLocation(Pos + 50.f);
		spawnedObject->SetObjectScale(SVector3(50.f,50.f,50.f));
	}

	if (EventName == "TestHit")
	{
		int i = 0;
	}
}

void DefaultAttackAbillity::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{

	if (DefaultAttack.size() > 0)
		AbilityUtils::PlayAnimation(this, DefaultAttack[m_ComboCount], false, false);

}

void DefaultAttackAbillity::OnCancelAbility()
{

}

void DefaultAttackAbillity::OnEndAbility()
{
	m_ComboCount = 0;
}

void DefaultAttackAbillity::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	m_CombeEnable = true;
}


