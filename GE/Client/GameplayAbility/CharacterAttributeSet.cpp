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
	// ������ ó��

	// Src�� AttackPower�� ����� ó���ؼ� Damage�� �о�����ϴµ� BackingAttribute�� ����ϸ� Target���� ó���ع����ϱ� ������

	// ó���� �������� �޾ƿ��� 0���� �о���
	float hitDamage = Damage.GetCurrentValue();
	Damage.SetCurrentValue(0.0f);



	//src �� Target ���� ȿ���� �ο�
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
	// �ִ� �̵��ӵ� Ŭ����
	if (Attribute.Attribute->GetName() == "MoveSpeed")
	{
		NewValue = SMathUtils::Clamp<float>(NewValue, 0, 500);

		// Movement�� Speed ����
		GetOwner<Character>()->GetMovementComponent()->SetMaxSpeed(NewValue);
	}

	// ������ Ŭ���� (�ּ� ������ 1)
	if (Attribute.Attribute->GetName() == "Damage")
	{
		NewValue = ::max<float>(1, NewValue);//Clamp<float>(NewValue, 1, 500);
	}
}
