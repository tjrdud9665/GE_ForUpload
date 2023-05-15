#include "EnginePCH.h"
#include "AbilityComponent.h"

AbilityComponent::AbilityComponent()
	:m_EffectCounter(0)
	, m_InputHandle(nullptr)
{

}

AbilityComponent::AbilityComponent(const AbilityComponent& Other)
	: Super(Other)
	, m_EffectCounter(0)
	, m_InputHandle(nullptr)
{

}

AbilityComponent::~AbilityComponent()
{
	std::list<SGameplayEffectSpec*>::iterator iter = m_ActiveGameplayEffectList.begin();
	std::list<SGameplayEffectSpec*>::iterator iterEnd = m_ActiveGameplayEffectList.end();

	for (; iter != iterEnd; iter++)
	{
		SAFE_DELETE(*iter);
	}
	m_AttirbuteSets.clear();

	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		SAFE_DELETE(m_ActivatableAbilities[i]->AbilityInstance);
		SAFE_DELETE(m_ActivatableAbilities[i]);
	}

	m_ActivatableAbilities.clear();
	m_EffectCounter = 0;
}

void AbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	m_AttirbuteSets.clear();

	XObject* Owner = GetOwner();
	if (Owner)
	{
		FPropertyMap props;
		Owner->GetClass()->GetAllProperties(OUT props);
		FPropertyMap::const_iterator iter = props.begin();
		FPropertyMap::const_iterator iterEnd = props.end();

		for (; iter != iterEnd; iter++)
		{
			if ((*iter).second->GetTypeName().find(AttributeSet::StaticClass()->GetName()) != std::string::npos)
			{
				TPropertyHandle<AttributeSet*>* handle = (TPropertyHandle<AttributeSet*>*)(*iter).second->CreateHandle(Owner);
				m_AttirbuteSets.push_back(*handle->GetValuePtr());
				SAFE_DELETE(handle);
			}
		}
	}
}

void AbilityComponent::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	std::list<SGameplayEffectSpec*>::iterator iter = m_ActiveGameplayEffectList.begin();
	std::list<SGameplayEffectSpec*>::iterator iterEnd = m_ActiveGameplayEffectList.end();

	for (; iter != iterEnd;)
	{
		if ((*iter)->Def->DurationPolicy.Get() == EGameplayEffectDurationType::HasDuration)
		{
			if ((*iter)->Def->Period > 0)
			{
				if ((*iter)->Period > 0)
				{
					(*iter)->Period -= DeltaSeconds;
				} else if ((*iter)->Period <= 0)
				{
					(*iter)->Period = (*iter)->Def->Period;
					//Period 를 초기하고 
					//Modifier 를 적용시킨다.
					size_t AttModeSize = (*iter)->Def->AttributeModifiers.size();

					for (size_t i = 0; i < AttModeSize; i++)
					{
						std::list<AttributeSet*>::iterator iter_att = m_AttirbuteSets.begin();
						std::list<AttributeSet*>::iterator iterEnd_att = m_AttirbuteSets.end();
						for (; iter_att != iterEnd_att; iter_att++)
						{
							float appliedVal = 0;


							if ((*iter)->Def->AttributeModifiers[i]->ApplyAttributeMod((*iter_att), GetAttributeSetFromClass(*iter_att), false, OUT appliedVal))
							{
								(*iter_att)->PostGameplayEffectExecute((*iter)->Def, this, (*iter)->Context.TargetComp);

								(*iter)->AppliedMod.push_back(SGameplayEffectModifiedAttribute((*iter)->Def->AttributeModifiers[i]->GetAttribute(), appliedVal));
								//spec.AppliedMod

							}
						}
					}
				}
			}


			(*iter)->Duration -= DeltaSeconds;
			if ((*iter)->Duration <= 0)
			{

				//적용시킨 모든 모디 파이어를 제거한다.
				size_t AttModeSize = (*iter)->Def->AttributeModifiers.size();



				for (size_t i = 0; i < AttModeSize; i++)
				{
					std::list<AttributeSet*>::iterator iter_att = m_AttirbuteSets.begin();
					std::list<AttributeSet*>::iterator iterEnd_att = m_AttirbuteSets.end();

					for (; iter_att != iterEnd_att; iter_att++)
					{
						size_t applied = (*iter)->AppliedMod.size();
						for (size_t z = 0; z < applied; z++)
						{
							if ((*iter)->Def->AttributeModifiers[i]->RemoveAppliedMod((*iter_att), (*iter)->AppliedMod[z]))
							{
								//Removed..
							}
						}

					}
				}

				SAFE_DELETE(*iter);
				iter = m_ActiveGameplayEffectList.erase((iter));
				iterEnd = m_ActiveGameplayEffectList.end();
				continue;
			}
		}
		iter++;
	}

}

void AbilityComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	size_t activatableSize = m_ActivatableAbilities.size();
	for (size_t i = 0; i < activatableSize; i++)
	{
		SkeletalMeshComponent* avatar = m_ActivatableAbilities[i]->ObjectInfo.OwnerAvatar;
		if (avatar)
		{
			GameplayAbility* instOrCDO = m_ActivatableAbilities[i]->AbilityInstance;
			if (!instOrCDO)
			{
				instOrCDO = m_ActivatableAbilities[i]->AbilityCDO;
			}
			if (instOrCDO->IsAbilityEndOnAnimationEnd() && avatar->IsAnimEnded())
			{
				if (avatar->GetCurrentAnimationSequence() == instOrCDO->m_ObjectInfo.PlayingAnimSeq)
				{
					instOrCDO->EndAbility();
				}
			}
		}
	}
}

bool AbilityComponent::GiveAbility(TSubclassOf<GameplayAbility> InGiveAbility)
{
	//만약 이미있는능력이 있다면 덮는다.
	if (!InGiveAbility.CustomCDOName.empty())
	{
		GameplayAbility* CDO = InGiveAbility->GetCustomCDO<GameplayAbility>(InGiveAbility.CustomCDOName);

		size_t ActivatableSize = m_ActivatableAbilities.size();
		for (size_t i = 0; i < ActivatableSize; i++)
		{
			if (CDO == m_ActivatableAbilities[i]->AbilityCDO)
			{
				//이미 존재 한다면 
				if (m_ActivatableAbilities[i]->AbilityCDO->m_InstancingPolicy.Get() == EGameplayAbilityInstancingPolicy::InstancedPerObject)
				{
					SAFE_DELETE(m_ActivatableAbilities[i]->AbilityInstance);
					m_ActivatableAbilities[i]->AbilityInstance = Globals::CreateX(InGiveAbility, this);
					//m_ActivatableAbilities[i].AbilityInstance->OnGiveAbility();

				}
			}
		}
		//없는 경우 생성
		//Has Sprite? ? 
		SkeletalMeshComponent* abilityAvatar = nullptr;
		if (GetOwner()->GetClass()->IsChildOf(Character::StaticClass()))
		{
			abilityAvatar = GetOwner<Character>()->GetSkeletalMeshComponent();
		}



		SGameplayAbilitySpec* newSpec = new SGameplayAbilitySpec(CDO, SGameplayAbilityObjectInfo(GetOwner<GameObject>(), abilityAvatar, this), nullptr);
		if (CDO->m_InstancingPolicy.Get() == EGameplayAbilityInstancingPolicy::InstancedPerObject)
		{
			newSpec->AbilityInstance = Globals::CreateX(InGiveAbility, this);
			
			
			//InstancedPerExecution 인경우 발동시 생성 -> 끝날시 Instance제거
		}
		m_ActivatableAbilities.push_back(newSpec);

		if(m_InputHandle)
		{
			newSpec->PressedID = m_InputHandle->AddActionBind(newSpec->AbilityInstance->BindKeyName, EInputEventType::IE_Pressed, std::bind(&AbilityComponent::AbilityInputPressed, this,  std::placeholders::_1, std::placeholders::_2));
			newSpec->ReleasedID = m_InputHandle->AddActionBind(newSpec->AbilityInstance->BindKeyName, EInputEventType::IE_Released, std::bind(&AbilityComponent::AbilityInputReleased, this , std::placeholders::_1, std::placeholders::_2));
		}


	}




	return true;
}

bool AbilityComponent::TryActivateAbilityByClass(TSubclassOf<GameplayAbility> InAbilityToActivate)
{
	size_t activatableSize = m_ActivatableAbilities.size();
	if (InAbilityToActivate.CustomCDOName.empty())
		return false;

	for (size_t i = 0; i < activatableSize; i++)
	{
		if (m_ActivatableAbilities[i]->AbilityCDO == InAbilityToActivate->GetCustomCDO(InAbilityToActivate.CustomCDOName))
		{
			return m_ActivatableAbilities[i]->AbilityInstance->CallActivateAbility(*m_ActivatableAbilities[i]);

		}
	}

	return true;
}

class GameplayAbility* AbilityComponent::GetAbilityInsance(TSubclassOf<GameplayAbility> InAbilityClass)
{
	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		if (m_ActivatableAbilities[i]->AbilityCDO == InAbilityClass->GetCustomCDO(InAbilityClass.CustomCDOName))
		{
			return m_ActivatableAbilities[i]->AbilityInstance;
		}

	}

	return nullptr;
}



void AbilityComponent::ModifyGameplayAbilitySpecInfo(class GameplayAbility* InAbilityInst, const SGameplayAbilityObjectInfo& Info)
{
	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		if (m_ActivatableAbilities[i]->AbilityInstance == InAbilityInst)
		{
			m_ActivatableAbilities[i]->ObjectInfo = Info;
			break;
		}

	}
}

void AbilityComponent::SetUpInput(InputHandle& handle)
{
	m_InputHandle = &handle;

	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		if (m_ActivatableAbilities[i]->PressedID == 0)
			m_ActivatableAbilities[i]->PressedID = m_InputHandle->AddActionBind(m_ActivatableAbilities[i]->AbilityInstance->BindKeyName, EInputEventType::IE_Pressed, std::bind(&AbilityComponent::AbilityInputPressed, this, std::placeholders::_1, std::placeholders::_2));

		if (m_ActivatableAbilities[i]->ReleasedID == 0)
			m_ActivatableAbilities[i]->ReleasedID =  m_InputHandle->AddActionBind(m_ActivatableAbilities[i]->AbilityInstance->BindKeyName, EInputEventType::IE_Released, std::bind(&AbilityComponent::AbilityInputReleased, this,  std::placeholders::_1, std::placeholders::_2));
		
	}
	
	//m_ActivatableAbilities
}

void AbilityComponent::AbilityInputPressed(const std::string& ActionName, uint64 pressedID)
{
	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		if (m_ActivatableAbilities[i]->PressedID == pressedID && ActionName == m_ActivatableAbilities[i]->AbilityCDO->BindKeyName)
		{	
			if(m_ActivatableAbilities[i]->AbilityInstance)
			{
				if(!m_ActivatableAbilities[i]->AbilityInstance->IsActivating())
				{
					m_ActivatableAbilities[i]->AbilityInstance->CallActivateAbility(*m_ActivatableAbilities[i]);					
				}
				else
				{
					m_ActivatableAbilities[i]->AbilityInstance->InputPressed(*m_ActivatableAbilities[i]);
				}
			
			}
			return;
		}
	}
}

void AbilityComponent::AbilityInputReleased(const std::string& ActionName, uint64 releasedID)
{
	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		if (m_ActivatableAbilities[i]->ReleasedID == releasedID && ActionName == m_ActivatableAbilities[i]->AbilityCDO->BindKeyName)
		{
			if (m_ActivatableAbilities[i]->AbilityInstance)
			{			
				m_ActivatableAbilities[i]->AbilityInstance->InputReleased(*m_ActivatableAbilities[i]);			
			}
			return;
		}
	}
}

void AbilityComponent::HandleGameplayEvent(const std::string& EvenName, SGameplayEventData*& EventData)
{
	size_t activatableSize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < activatableSize; i++)
	{
		m_ActivatableAbilities[i]->AbilityInstance->OnReceivedGameplayEvent(EvenName, EventData, *m_ActivatableAbilities[i]);
	}
}


bool AbilityComponent::CanApplyAttributeModifiers(TSubclassOf<GameplayEffect> CostGameplayEffectClass)
{
	SGameplayEffectSpec* spec = nullptr;
	if (CostGameplayEffectClass)
	{
		GameplayEffect* CDO = (GameplayEffect*)CostGameplayEffectClass->GetCustomCDO(CostGameplayEffectClass.CustomCDOName);

		if (CDO)
		{
			size_t AttModeSize = CDO->AttributeModifiers.size();
			for (size_t i = 0; i < AttModeSize; i++)
			{
				std::list<AttributeSet*>::iterator iter = m_AttirbuteSets.begin();
				std::list<AttributeSet*>::iterator iterEnd = m_AttirbuteSets.end();
				for (; iter != iterEnd; iter++)
				{
					if(CDO->AttributeModifiers[i]->ModiferOp.Get() == EGameplayModOp::Additive)
					{
						continue;
					}
					float CurrentValue = CDO->AttributeModifiers[i]->Attribute.GetCurrentValue(*iter);
					float CostValue = CDO->AttributeModifiers[i]->ModiferMagnitude;				

					if (CurrentValue + CostValue < 0.f)
					{
						return false;
					}
				}
			}
			
		}

	}


	return true;
}

float AbilityComponent::GetAttribute(const std::string& InAttributeName)
{
	float CurrentValue = 0;

	std::list<AttributeSet*>::iterator iter_att = m_AttirbuteSets.begin();
	std::list<AttributeSet*>::iterator iterEnd_att = m_AttirbuteSets.end();

	FPropertyMap props;
	for (; iter_att != iterEnd_att; iter_att++)
	{
		(*iter_att)->GetClass()->GetAllProperties(OUT props);
		FPropertyMap::iterator propIter = props.find(InAttributeName);

		if (propIter != props.end())
		{

			PropertyHandle* handle = propIter->second->CreateHandle((*iter_att));
			TPropertyHandle<SGameplayAttributeData>* propHandle = (TPropertyHandle<SGameplayAttributeData>*)handle;
			if (handle)
			{
				CurrentValue = propHandle->GetValuePtr()->CurrentValue;
			}
			SAFE_DELETE(handle);

			break;
		}
	}

	return CurrentValue;
}

float AbilityComponent::GetEffectRemainingTime(uint64 InHandle)
{
	std::list<SGameplayEffectSpec*>::iterator iter = m_ActiveGameplayEffectList.begin();
	std::list<SGameplayEffectSpec*>::iterator iterEnd = m_ActiveGameplayEffectList.end();

	for (; iter != iterEnd; iter++)
	{
		if (InHandle == (*iter)->Handle)
		{
			return (*iter)->GetDuration();
		}
	}

	return 0;
}

int32 AbilityComponent::GetActivatingAbility() const
{
	int32 ActivatingCount = 0;
	size_t abilitySize = m_ActivatableAbilities.size();

	for (size_t i = 0; i < abilitySize; i++)
	{
		GameplayAbility* abilityInst = m_ActivatableAbilities[i]->AbilityInstance;
		if (abilityInst)
		{
			if(abilityInst->IsActivating())
				ActivatingCount++;
		}

	}

	return ActivatingCount;
}

AttributeSet* AbilityComponent::GetAttributeSetFromClass(class AttributeSet* src)
{
	std::list<AttributeSet*>::iterator iter = m_AttirbuteSets.begin();
	std::list<AttributeSet*>::iterator iter_end = m_AttirbuteSets.end();

	for(;iter != iter_end;iter++)
	{
		if((*iter)->GetClass() == src->GetClass() || src->GetClass()->IsChildOf((*iter)->GetClass()))// (*iter)->GetClass()->IsChildOf(src->GetClass()))
		{
			return (*iter);
		}
	}

	return nullptr;

}

SGameplayEffectSpec* AbilityComponent::ApplyGameplayEffectToTarget(TSubclassOf<GameplayEffect> GameplayEffectClass, AbilityComponent* Target)
{
	SGameplayEffectSpec* spec = nullptr;
	if (GameplayEffectClass)
	{
		GameplayEffect* CDO = (GameplayEffect*)GameplayEffectClass->GetCustomCDO(GameplayEffectClass.CustomCDOName);

		if (CDO)
		{
			if (CDO->DurationPolicy.Get() != EGameplayEffectDurationType::Instant)
			{

				GameplayEffect* createdEffect = Globals::CreateX(GameplayEffectClass, this);

				SGameplayEffectContext context;
				context.AbilityLevel = 1;
				context.EffectCauser = GetOwner<GameObject>();
				context.Instigator = GetOwner<GameObject>();
				context.InstigatorAbilityComponent = this;
				context.WorldOrigin = GetOwner<GameObject>()->GetWorldLocation();
				context.TargetComp = Target;

				spec = new SGameplayEffectSpec(createdEffect, context);
				spec->Handle = ++m_EffectCounter;
				size_t AttModeSize = createdEffect->AttributeModifiers.size();



				for (size_t i = 0; i < AttModeSize; i++)
				{
					std::list<AttributeSet*>::iterator iter = Target->m_AttirbuteSets.begin();
					std::list<AttributeSet*>::iterator iterEnd = Target->m_AttirbuteSets.end();

					for (; iter != iterEnd; iter++)
					{
						float appliedVal = 0;

						if (createdEffect->AttributeModifiers[i]->ApplyAttributeMod((*iter), GetAttributeSetFromClass(*iter), false, OUT appliedVal))
						{
							(*iter)->PostGameplayEffectExecute(CDO, this, Target);
							spec->AppliedMod.push_back(SGameplayEffectModifiedAttribute(createdEffect->AttributeModifiers[i]->GetAttribute(), appliedVal));


						}
					}
				}

				m_ActiveGameplayEffectList.push_back(spec);

			}
			//Instant
			else
			{
				//바로 Modifier 를 적용			
				size_t AttModeSize = CDO->AttributeModifiers.size();

				for (size_t i = 0; i < AttModeSize; i++)
				{
					std::list<AttributeSet*>::iterator iter = Target->m_AttirbuteSets.begin();
					std::list<AttributeSet*>::iterator iterEnd = Target->m_AttirbuteSets.end();
					for (; iter != iterEnd; iter++)
					{
						float appliedVal = 0; 
						//GetAttributeSetFromClass((*iter));
						if (CDO->AttributeModifiers[i]->ApplyAttributeMod((*iter),GetAttributeSetFromClass(*iter) ,true, appliedVal))
						{
							(*iter)->PostGameplayEffectExecute(CDO, this, Target);
						}
					}
				}
			}
		}

	}


	return spec;
}

SGameplayEffectSpec* AbilityComponent::ApplyGameplayEffectToSelt(TSubclassOf<GameplayEffect> GameplayEffectClass)
{
	return ApplyGameplayEffectToTarget(GameplayEffectClass, this);
}
