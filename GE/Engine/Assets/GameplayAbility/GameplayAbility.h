#pragma once
#include "../AssetData.h"
#include <GameplaySystem/GameplaySystem.h>
#include "../GameplayEffect/GameplayEffect.h"

struct SGameplayAbilityObjectInfo
{
	class GameObject* OwnerObject;
	class AbilityComponent* OwnerComponent;
	class SkeletalMeshComponent* OwnerAvatar;

	class AnimationSequence* PlayingAnimSeq;

	SGameplayAbilityObjectInfo(class GameObject* InOwnerObject, class SkeletalMeshComponent* InAvatar, class AbilityComponent* InOwnerComp)
		:OwnerObject(InOwnerObject)
		, OwnerComponent(InOwnerComp)
		, OwnerAvatar(InAvatar)
		, PlayingAnimSeq(nullptr)
	{

	}
	SGameplayAbilityObjectInfo()
		:OwnerObject(nullptr)
		, OwnerComponent(nullptr)
		, OwnerAvatar(nullptr)
		, PlayingAnimSeq(nullptr)
	{

	}
};

struct SGameplayAbilitySpec
{
	SGameplayAbilitySpec()
	{

	}



	class GameplayAbility* AbilityCDO;
	int32				Level;	
	uint64				PressedID;
	uint64				ReleasedID;

	class GameplayAbility* AbilityInstance;

	SGameplayAbilityObjectInfo	ObjectInfo;
	

	SGameplayAbilitySpec(class GameplayAbility* CDO, const SGameplayAbilityObjectInfo& ObjInfo, class GameplayAbility* Instance, int32 InLevel = 0)
		:AbilityCDO(CDO)
		, ObjectInfo(ObjInfo)
		, AbilityInstance(Instance)
		, Level(InLevel)
		, PressedID(0)
		,ReleasedID(0)
	{
		
	}

};

/*
* 이클래스에서는 단순히 어빌리티의 발동 가능여부 및 발동시에 기본적용되는 효과( Cost, CoolTime)
* 및 발동금지조건(Block) 및 사용도중 interrupt(스킬 정지)등의 제어를 한다.
*/

//Call Only Ability
class AbilityUtils
{	
public:
	static void PlayAnimation(class GameplayAbility* InAbility, class AnimationSequence* InAnimSeq, bool bLoop, bool bRootMotionEable,bool PlayFromStart,bool IsReverse = false);
};



class GameplayAbility :public AssetData
{
	CLASS(GameplayAbility, AssetData, Abstract)	//생성불가

	friend class AbilityComponent;
	friend class GameplayEffect;
	friend class AbilityUtils;


protected:
	GameplayAbility();
	GameplayAbility(const GameplayAbility& Other);
	virtual ~GameplayAbility();

	bool CallActivateAbility(const SGameplayAbilitySpec& InSpec);

	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec);


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec);
	

public:
	void PreActivate(const SGameplayAbilitySpec& InSpec);
	bool ActivateAbility(const SGameplayAbilitySpec& InSpec);

	virtual bool CommitAbility(const SGameplayAbilitySpec& InSpec);

	virtual bool CommitCheck(const SGameplayAbilitySpec& InSpec);



	bool CheckCooldown(const SGameplayAbilitySpec& InSpec);

	bool CheckCost(const SGameplayAbilitySpec& InSpec);

	void CommitExecute(const SGameplayAbilitySpec& InSpec);		//ApplyCoolDown, ApplyCost)

	void ApplyCoolDown(const SGameplayAbilitySpec& InSpec);

	void ApplyCost(const SGameplayAbilitySpec& InSpec);

	bool CheckActivationTags(const SGameplayAbilitySpec& InSpec);


	void EndAbility();
	virtual void OnEndAbility();

	bool IsActivating() const;


	void CancelAbility();
	virtual void OnCancelAbility();



	//virtual void ActivateAbility(const SGameplayAbilitySpec& abilitySpec);
	//virtual bool CanActivateAbility(/* Some Context Of This Ability*/);
	//virtual bool CommitAbility();		//Check Cost		 and Cooltime
	//bool CheckCost() const;
	//bool CheckCooltime() const;	
	//virtual void CancelAbility();
	//virtual void EndAbility();
	//float GetCooldownTimeRemaining() const;

	bool IsAbilityEndOnAnimationEnd() const;

	uint64 GetCoolTimeID()	const;

	

private:
	virtual void InputPressed(const SGameplayAbilitySpec& InSpec);
	virtual void InputReleased(const SGameplayAbilitySpec& InSpec);

protected:
	virtual bool PostLoad() override;

	virtual void SetOwner(XObject* InOwner) override;

protected:
	PROP(TSubclassOf<GameplayEffect>, CoolTime, DetailsEdit)
		TSubclassOf<GameplayEffect> CoolTime;

	uint64		m_CoolTimeID;

	PROP(TSubclassOf<GameplayEffect>, CostGameplayEffectClass, DetailsEdit)
		TSubclassOf<GameplayEffect>	CostGameplayEffectClass;


	PROP(TEnum<EGameplayAbilityInstancingPolicy>, m_InstancingPolicy, DetailsEdit)
		TEnum<EGameplayAbilityInstancingPolicy>	m_InstancingPolicy;

	PROP(bool, AbilityEndOnAnimEnd, DetailsEdit)
	bool AbilityEndOnAnimEnd;

	SGameplayAbilityObjectInfo	m_ObjectInfo;

	PROP(std::string, BindKeyName, DetailsEdit)
		std::string BindKeyName;	


private:
	class Scene* m_OwningScene;

protected:
	bool		m_Activated;

	PROP(std::vector<std::string>, ActivationBlockTag, DetailsEdit)
	std::vector<std::string> ActivationBlockTag;

	PROP(std::vector<std::string>, ActivationRequireTag, DetailsEdit)
	std::vector<std::string> ActivationRequireTag;
};

