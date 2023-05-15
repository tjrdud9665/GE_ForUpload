#include "PCH.h"
#include "GameplayAbility_Attack.h"
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <ObjectManager.h>
#include <GameplaySystem/GameplaySystem.h>
#include <CameraManager.h>
#include <GameFramework/Components/CameraComponent.h>

GameplayAbility_Attack::GameplayAbility_Attack()
	: SpawnOffset(0.f)
	, AnimPlayRate(1.f)
	, ChasingMouse(false)
{
}

GameplayAbility_Attack::GameplayAbility_Attack(const GameplayAbility_Attack& Other)
	: Super(Other)
	, AnimPlayRate(Other.AnimPlayRate)
	, RootMotion(Other.RootMotion)
	, HitEffect(Other.HitEffect)
	, HitBoxClass(Other.HitBoxClass)
	, SpawnOffset(Other.SpawnOffset)
	, ChasingMouse(Other.ChasingMouse)
	, ParticleClass(Other.ParticleClass)
{
	size_t ComboCount = Other.Attacks.size();
	Attacks.resize(ComboCount);

	for (size_t i = 0; i < ComboCount; ++i)
	{
		Attacks[i] = Other.Attacks[i];
	}
}

GameplayAbility_Attack::~GameplayAbility_Attack()
{
}

void GameplayAbility_Attack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (IsActivating())
	{
		if ("End" == EventName)
		{
			EndAbility();
		}

		// Spawn Event가 들어왔을 시 (Animation에서 공격타이밍에 Hitbox를 소환하라는 메세지)
		else if ("Spawn" == EventName)
		{
			HitBoxObject* spawnedHitBox = nullptr;

			if (HitBoxClass)
				spawnedHitBox = g_Engine->Get<ObjectManager>()->CreateGameObject<HitBoxObject>(HitBoxClass);


			if (spawnedHitBox)
			{
				SVector3 Pos = m_ObjectInfo.OwnerObject->GetWorldLocation();
				SVector3 Dir = m_ObjectInfo.OwnerObject->GetObjectForwardVector();
				SVector3 Rot = m_ObjectInfo.OwnerObject->GetWorldRotation();

				spawnedHitBox->SetShooter(m_ObjectInfo.OwnerObject);

				spawnedHitBox->SetObjectLocation(Pos + Dir * SpawnOffset);
				spawnedHitBox->SetObjectRotation(SVector3(0.f, 0.f, Rot.y));


				g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(spawnedHitBox);
			}


		}

		// KnockDown
		else if ("KnockDown" == EventName)
		{
			if (!EventData)
				return;

			IAbilitySystem* abilitySys = dynamic_cast<IAbilitySystem*>(EventData->Target);

			if (abilitySys)
			{

				AbilityComponent* targetComp = abilitySys->GetAbilityComponent();
				AbilityComponent* ShooterComp = m_ObjectInfo.OwnerComponent;

				targetComp->GetOwner<Character>()->AddTag("KnockDown", true);

				ShooterComp->ApplyGameplayEffectToTarget(HitEffect, targetComp);

			}

			// 만약 공격됐을 때 발생할 Effect가 있다면 여기서 추가
			ParticleObject* ParticleEffect = nullptr;

			if (ParticleClass)
				ParticleEffect = g_Engine->Get<ObjectManager>()->CreateGameObject<ParticleObject>(ParticleClass);

			if (ParticleEffect)
			{
				ParticleEffect->SetObjectLocation(EventData->Target->GetRootComponent()->GetWorldLocation());

				g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(ParticleEffect);
			}
		}
		
		// Hit
		else if ("Hit" == EventName)
		{
			if (!EventData)
				return;

			IAbilitySystem* abilitySys = dynamic_cast<IAbilitySystem*>(EventData->Target);

			if (abilitySys)
			{

				AbilityComponent* targetComp = abilitySys->GetAbilityComponent();
				AbilityComponent* ShooterComp = m_ObjectInfo.OwnerComponent;

				ShooterComp->ApplyGameplayEffectToTarget(HitEffect, targetComp);
			}

			// 만약 공격됐을 때 발생할 Effect가 있다면 여기서 추가
			ParticleObject* ParticleEffect = nullptr;

			if (ParticleClass)
				ParticleEffect = g_Engine->Get<ObjectManager>()->CreateGameObject<ParticleObject>(ParticleClass);

			if (ParticleEffect)
			{
				ParticleEffect->SetObjectLocation(EventData->Target->GetRootComponent()->GetWorldLocation());

				g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject(ParticleEffect);
			}



		}

		// 쳐맞았을 때
		else if ("Hurt" == EventName)
		{
			CancelAbility();
		}
	}
}

void GameplayAbility_Attack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	//Active시에 첫번 째 공격을 수행
	//m_ObjectInfo.OwnerObject->AddTag("CantMove", true);
	m_ObjectInfo.OwnerObject->AddTag("CanAbility", false);

	if (ChasingMouse)
		ChangeDirection();

	m_ObjectInfo.OwnerAvatar->SetPlayRate(AnimPlayRate);
	AbilityUtils::PlayAnimation(this, Attacks[0], false, RootMotion, true);
}

void GameplayAbility_Attack::OnEndAbility()
{
	//if (m_ObjectInfo.OwnerObject)
	//{
	//	m_ObjectInfo.OwnerObject->AddTag("CantMove", false);
	//	m_ObjectInfo.OwnerObject->AddTag("CanAbility", true);
	//}
}

void GameplayAbility_Attack::OnCancelAbility()
{
	//if (m_ObjectInfo.OwnerObject)
	//{
	//	m_ObjectInfo.OwnerObject->AddTag("CantMove", false);
	//	m_ObjectInfo.OwnerObject->AddTag("CanAbility", true);
	//}
}

void GameplayAbility_Attack::ChangeDirection()
{
	D3D11_VIEWPORT viewport = g_Engine->Get<Renderer>()->GetCurrentViewport();

	if (g_Engine->IsEditMode())
	{
		viewport = g_Engine->GetEditViewport();
	}

	SVector2 mousePos = g_Engine->Get<Input>()->GetMousePosition();

	SVector3 origin;
	SVector3 direction;

	// Ray Picking
	SHitResult hit = g_Engine->Get<CameraManager>()->GetCurrentCamera()->pickRay(viewport, mousePos.x, mousePos.y, OUT origin, OUT direction);

	SVector3 End = hit.point;

	SVector3 Rot = SMathUtils::FindLookAtRotation(m_ObjectInfo.OwnerObject->GetWorldLocation(), End);

	m_ObjectInfo.OwnerObject->SetWorldRotation(SVector3(0.f, 0.f, Rot.y));
}
