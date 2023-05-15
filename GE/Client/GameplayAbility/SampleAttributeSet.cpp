#include "PCH.h"
#include "SampleAttributeSet.h"
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/MovementComponent.h>
#include <GameFramework/Components/CharacterMovementComponent.h>

SampleAttributeSet::SampleAttributeSet()
{
	
}

SampleAttributeSet::SampleAttributeSet(const SampleAttributeSet& Other)
	:Super(Other)
{

}

SampleAttributeSet::~SampleAttributeSet()
{

}

void SampleAttributeSet::PreAttributeChange(const SGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute.Attribute->GetName() == "MoveSpeed")
	{
		NewValue = SMathUtils::Clamp<float>(NewValue, 0, 1000);
		GetOwner<Character>()->GetMovementComponent()->SetMaxSpeed(NewValue);
	}
}

void SampleAttributeSet::PostGameplayEffectExecute(class GameplayEffect* InDef, class AbilityComponent* Src, class AbilityComponent* Target)
{
	float hitDamage = Damage.GetCurrentValue();
	Damage.SetCurrentValue(0.0f);



	//src 가 Target 에게 효과를 부여
	Character* ownerChar = GetOwner<Character>();
	GameObject* damageCauser = nullptr;
	if (Src)
		damageCauser = Src->GetOwner<GameObject>();

	if (ownerChar)
	{
		if (hitDamage)
		{
			hitDamage = std::max(1.0f, hitDamage - DefensePower.GetCurrentValue());
			ownerChar->OnDamaged(hitDamage, damageCauser);

			float newHealth = std::max(0.0f, Health.GetCurrentValue() - hitDamage);
			Health.SetCurrentValue(newHealth);

			Character* causerChar = (Character*)damageCauser;

			//if (causerChar)
			//{
			//	causerChar->SetLastTarget(ownerChar);
			//}

			if (newHealth == 0)
			{
				ownerChar->Die();
			}
		}


	}
}
