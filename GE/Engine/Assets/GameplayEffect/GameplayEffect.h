#pragma once
#include "GameplaySystem/GameplaySystem.h"
#include "GameplaySystem/AttributeSet.h"
#include "../AssetData.h"


//EGameplayEffectMagnitudeCalculation
//AttributeBased, ScalableFloat 2가지만 지원




class AttributeModifer : public XObject
{
	CLASS(AttributeModifer, XObject)
	friend class GameplayEffect;
	friend class ArrayPropertyWidget;
	friend class AbilityComponent;

public:
	AttributeModifer()
		:ModiferOp(EGameplayModOp::Additive)
		, ModCalcType(EGameplayModCalcType::StaticFloat)
		, ModiferMagnitude(0.0f)
		, Coefficient(0.0f)
		, PreMultiplyAdditiveValue(0.0f)
		, PostMultiplyAdditiveValue(0.0f)
		, AttribuetSource(EAttributeSource::Target)
	{
		//PreConstruct(this, StaticClass(), this);
		//Construct("AttributeModifer");

	}
	AttributeModifer(const AttributeModifer& Other)
		:Super(Other)
		, ModiferOp(Other.ModiferOp)
		, ModCalcType(Other.ModCalcType)
		, ModiferMagnitude(Other.ModiferMagnitude)
		, Attribute(Other.Attribute)
		, BackingAttribute(Other.BackingAttribute)
		, Coefficient(Other.Coefficient)
		, PreMultiplyAdditiveValue(Other.PreMultiplyAdditiveValue)
		, PostMultiplyAdditiveValue(Other.PostMultiplyAdditiveValue)
		,AttribuetSource(Other.AttribuetSource)
	{

	}
	virtual ~AttributeModifer()
	{

	}

	bool RemoveAppliedMod(class AttributeSet* target, const SGameplayEffectModifiedAttribute& Moded)
	{
		SGameplayAttributeData* attributeData = Moded.Attribute.GetGameplayAttributeData(target);
		if (attributeData)
		{

			float Val = Moded.TotalMagnitude;
			float prevVal = attributeData->GetCurrentValue() - Val;

			target->PreAttributeChange(Moded.Attribute, prevVal);

			attributeData->SetCurrentValue(prevVal);
			return true;
		}

		return false;


	}

	/* ModBase -> Base 값을 수정할것인가?*/
	bool  ApplyAttributeMod(class AttributeSet* applyTarget, class AttributeSet* applySource,bool ModBase, float& AppliedValue)
	{
		//Need BaseValue
		SGameplayAttributeData* attributeData = Attribute.GetGameplayAttributeData(applyTarget);
		if (attributeData)
		{
			float EVal = 0;
			if (ModCalcType.Get() == EGameplayModCalcType::StaticFloat)
			{
				EVal = ModiferMagnitude;
			} else
			{		
				AttributeSet* backingAtt = nullptr;

				if(AttribuetSource.Get() == EAttributeSource::Source)
				{
					backingAtt = applySource;
				}
				else
				{
					backingAtt = applyTarget;
				}

				SGameplayAttributeData* backingAttributeData = BackingAttribute.GetGameplayAttributeData(backingAtt);
				if (backingAttributeData)
				{
					EVal = Coefficient * (PreMultiplyAdditiveValue + backingAttributeData->BaseValue) + PostMultiplyAdditiveValue;
				}
			}

			float NewValue = 0;
			AppliedValue = 0;
			switch (ModiferOp.Get())
			{
			case EGameplayModOp::Additive:
				if (ModBase)
				{
					attributeData->BaseValue += EVal;
				}
				NewValue = attributeData->CurrentValue + EVal;
				break;
			case EGameplayModOp::Multiplicitive:
				if (ModBase)
				{
					attributeData->BaseValue *= EVal;
				}
				NewValue = attributeData->CurrentValue * EVal;
				break;
			case EGameplayModOp::Division:
				if (ModBase)
				{
					attributeData->BaseValue /= std::max(0.001f, EVal);
				}
				NewValue = attributeData->CurrentValue / std::max(0.001f, EVal);
				break;
			case EGameplayModOp::Override:
				if (ModBase)
				{
					attributeData->BaseValue = EVal;
				}
				NewValue = EVal;//attributeData->CurrentValue  
				break;
			}

			AppliedValue = NewValue - attributeData->CurrentValue;

			applyTarget->PreAttributeChange(Attribute, NewValue);		//Check And Clmap.



			attributeData->CurrentValue = NewValue;//Apply

			return true;
		}


		return false;
	}

public:
	const SGameplayAttribute& GetAttribute() const
	{
		return Attribute;
	}


private:

	PROP(SGameplayAttribute, Attribute, DetailsEdit)			//어떤 능력치를 
		SGameplayAttribute	Attribute;

	PROP(TEnum<EGameplayModOp>, ModiferOp, DetailsEdit)			//어떤 연산으로
		TEnum<EGameplayModOp>	ModiferOp;


	PROP(TEnum<EGameplayModCalcType>, ModCalcType, DetailsEdit)			//어떤 연산으로
		TEnum<EGameplayModCalcType> ModCalcType;
	//ModCalcType == StaticFloat 
	PROP(float, ModiferMagnitude, DetailsEdit)					//얼마 만큼
		float ModiferMagnitude;

	//ModifierManitude는 고정치가아닌 
	//BackingAttrubute에 기반한 값이 될수도 있다.
	// (Coefficient * (PreMultiplyAdditiveValue + [Attribute Value ])) + PostMultiplyAdditiveValue

	//ModCalcType == AttributeBased 
	PROP(TEnum<EAttributeSource>, AttribuetSource, DetailsEdit)			//누구의 ?
	TEnum<EAttributeSource> AttribuetSource;

	PROP(SGameplayAttribute, BackingAttribute, DetailsEdit)			//어떤 능력치의  
		SGameplayAttribute	BackingAttribute;



	PROP(float, Coefficient, DetailsEdit)							//계수
		float	Coefficient;

	PROP(float, PreMultiplyAdditiveValue, DetailsEdit)
		float PreMultiplyAdditiveValue;

	PROP(float, PostMultiplyAdditiveValue, DetailsEdit)
		float PostMultiplyAdditiveValue;


};

/*
* GameplayEffect는 적용시에 TargetActor의 AttributeSet의 Attribute 를 조작할 수 있어야한다.
* ApplyGameplayEffect -> MakeEffectContext
*/

class GameplayEffect :public AssetData
{
	CLASS(GameplayEffect, AssetData)
	friend struct SGameplayEffectSpec;
	friend class AbilityComponent;
protected:
	GameplayEffect();
	GameplayEffect(const GameplayEffect& Other);
	virtual ~GameplayEffect();

public:
	virtual bool PostLoad() override;

	virtual void SetOwner(XObject* InOwner) override;

protected:
	PROP(TEnum<EGameplayEffectDurationType>, DurationPolicy, DetailsEdit)
		TEnum<EGameplayEffectDurationType>	DurationPolicy;

	PROP(float, DurationMagnitude, DetailsEdit)			//Attribute 에 기반한 DuartionMagnitude를 제작 할지?
		float DurationMagnitude;

	PROP(float, Period, DetailsEdit)
		float Period;

	PROP(std::vector<AttributeModifer*>, AttributeModifiers, DetailsEdit)
		std::vector<AttributeModifer*>	AttributeModifiers;

	//	PROP()
	//	float DurtaionMagnitude;

	//	period
	//		Duration 타입의 경우  DurtaionMagnitude 시간동안 Period 마다 Modifier 적용
	//		단 Period == 0 인경우에는 시작할 때  한번 적용되고 끝날떄 수거해나감

	//		만약 Infinite 인 겨우에는 Period 마다 Apply 시킴


private:
	class Scene* m_OwningScene;


};

