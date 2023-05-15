#include "PCH.h"
#include "CharacterAttributeSet.h"
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/CharacterMovementComponent.h>

CharacterAttributeSet::CharacterAttributeSet()
{
}

CharacterAttributeSet::CharacterAttributeSet(const CharacterAttributeSet& Other)
	: Super(Other)
	, Health(Other.Health)
	, MaxHealth(Other.MaxHealth)
	, MoveSpeed(Other.MoveSpeed)
	, Damage(Other.Damage)
	, AttackPower(Other.AttackPower)
	, DefensePower(Other.DefensePower)
{
}

CharacterAttributeSet::~CharacterAttributeSet()
{
}

float CharacterAttributeSet::GetHealth() const
{
	return Health.CurrentValue;
}

float CharacterAttributeSet::GetMaxHealth() const
{
	return MaxHealth.CurrentValue;
}

float CharacterAttributeSet::GetMoveSpeed() const
{
	return MoveSpeed.CurrentValue;
}

float CharacterAttributeSet::GetAttackPower() const
{
	return AttackPower.CurrentValue;
}

float CharacterAttributeSet::GetDamage() const
{
	return Damage.CurrentValue;
}

float CharacterAttributeSet::GetDefensePower() const
{
	return DefensePower.CurrentValue;
}

void CharacterAttributeSet::PostGameplayEffectExecute(GameplayEffect* InDef, AbilityComponent* Src, AbilityComponent* Target)
{
	// 데미지 처리

	// Src의 AttackPower를 계수로 처리해서 Damage에 밀어줘야하는데 BackingAttribute를 사용하면 Target껄로 처리해버리니까 문제임

	// 처리된 데미지를 받아오고 0으로 밀어줌
	float hitDamage = Damage.GetCurrentValue();
	Damage.SetCurrentValue(0.0f);



	//src 가 Target 에게 효과를 부여
	Character* ownerChar = GetOwner<Character>();
	GameObject* damageCauser = nullptr;
	if (Src)
		damageCauser = Src->GetOwner<GameObject>();

	if (ownerChar)
	{
		bool isEvade = false;
		ownerChar->FindTag("Evade", OUT isEvade);

		if (!isEvade)
		{
			if (hitDamage)
			{
				hitDamage = std::max(1.0f, hitDamage - DefensePower.GetCurrentValue());
				ownerChar->OnDamaged(hitDamage, damageCauser);

				float newHealth = std::max(0.0f, Health.GetCurrentValue() - hitDamage);
				Health.SetCurrentValue(newHealth);

				if (newHealth == 0)
				{
					ownerChar->Die();
				}
			}
		}

	}
}

void CharacterAttributeSet::PreAttributeChange(const SGameplayAttribute& Attribute, float& NewValue)
{
	// 최대 이동속도 클램핑
	if (Attribute.Attribute->GetName() == "MoveSpeed")
	{
		NewValue = SMathUtils::Clamp<float>(NewValue, 0, 500);

		// Movement에 Speed 전달
		GetOwner<Character>()->GetMovementComponent()->SetMaxSpeed(NewValue);
	}

	// 데미지 클램핑 (최소 데미지 1)
	if (Attribute.Attribute->GetName() == "Damage")
	{
		NewValue = ::max<float>(1, NewValue);//Clamp<float>(NewValue, 1, 500);
	}
}
