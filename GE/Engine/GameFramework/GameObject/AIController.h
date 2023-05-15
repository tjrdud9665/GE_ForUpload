#pragma once
#include "Controller.h"
#include "../../Assets/AI/BehaviorTree.h"

class AIController :public Controller
{
	CLASS(AIController,Controller)

protected:
	AIController();
	AIController(const AIController& Other);
	virtual ~AIController();

public:
	void StartTree();

	virtual void Update(float DeltaSeconds) override;
	
	void StopTree();

	virtual void BeginPlay() override;

	virtual void OnPossess(class Character* InCharacter)override;

	bool SetBlackboardValueAsBool(const std::string& InKeyName, bool Value);

	bool SetBlackboardValueAsInt(const std::string& InKeyName, int32 Value);

	bool SetBlackboardValueAsFloat(const std::string& InKeyName, float Value);

	bool SetBlackboardValueAsVector(const std::string& InKeyName, const SVector3& Value);

	bool SetBlackboardValueAsObject(const std::string& InKeyName, XObject* Value);

	bool SetBlackboardValueAsAbility(const std::string& InKeyName, TSubclassOf<GameplayAbility> Value);



	bool RemoveBlackboardValue(const std::string& InKeyName,EBlackBoardValType valueType);


protected:
	PROP(TSharedPtr<class BehaviorTree>, BTree, DetailsEdit)
	TSharedPtr<class BehaviorTree> BTree;

	class BehaviorTree* BTreeInst;

	class Blackboard* BlackBoardInst;
};

