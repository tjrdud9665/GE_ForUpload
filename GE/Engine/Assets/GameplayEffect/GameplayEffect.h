#pragma once
#include "GameplaySystem/GameplaySystem.h"
#include "GameplaySystem/AttributeSet.h"
#include "../AssetData.h"


//EGameplayEffectMagnitudeCalculation
//AttributeBased, ScalableFloat 2������ ����




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

	/* ModBase -> Base ���� �����Ұ��ΰ�?*/
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

	PROP(SGameplayAttribute, Attribute, DetailsEdit)			//� �ɷ�ġ�� 
		SGameplayAttribute	Attribute;

	PROP(TEnum<EGameplayModOp>, ModiferOp, DetailsEdit)			//� ��������
		TEnum<EGameplayModOp>	ModiferOp;


	PROP(TEnum<EGameplayModCalcType>, ModCalcType, DetailsEdit)			//� ��������
		TEnum<EGameplayModCalcType> ModCalcType;
	//ModCalcType == StaticFloat 
	PROP(float, ModiferMagnitude, DetailsEdit)					//�� ��ŭ
		float ModiferMagnitude;

	//ModifierManitude�� ����ġ���ƴ� 
	//BackingAttrubute�� ����� ���� �ɼ��� �ִ�.
	// (Coefficient * (PreMultiplyAdditiveValue + [Attribute Value ])) + PostMultiplyAdditiveValue

	//ModCalcType == AttributeBased 
	PROP(TEnum<EAttributeSource>, AttribuetSource, DetailsEdit)			//������ ?
	TEnum<EAttributeSource> AttribuetSource;

	PROP(SGameplayAttribute, BackingAttribute, DetailsEdit)			//� �ɷ�ġ��  
		SGameplayAttribute	BackingAttribute;



	PROP(float, Coefficient, DetailsEdit)							//���
		float	Coefficient;

	PROP(float, PreMultiplyAdditiveValue, DetailsEdit)
		float PreMultiplyAdditiveValue;

	PROP(float, PostMultiplyAdditiveValue, DetailsEdit)
		float PostMultiplyAdditiveValue;


};

/*
* GameplayEffect�� ����ÿ� TargetActor�� AttributeSet�� Attribute �� ������ �� �־���Ѵ�.
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

	PROP(float, DurationMagnitude, DetailsEdit)			//Attribute �� ����� DuartionMagnitude�� ���� ����?
		float DurationMagnitude;

	PROP(float, Period, DetailsEdit)
		float Period;

	PROP(std::vector<AttributeModifer*>, AttributeModifiers, DetailsEdit)
		std::vector<AttributeModifer*>	AttributeModifiers;

	//	PROP()
	//	float DurtaionMagnitude;

	//	period
	//		Duration Ÿ���� ���  DurtaionMagnitude �ð����� Period ���� Modifier ����
	//		�� Period == 0 �ΰ�쿡�� ������ ��  �ѹ� ����ǰ� ������ �����س���

	//		���� Infinite �� �ܿ쿡�� Period ���� Apply ��Ŵ


private:
	class Scene* m_OwningScene;


};

