#include "EnginePCH.h"
#include "AIController.h"
#include "Character.h"

AIController::AIController()
{

}

AIController::AIController(const AIController& Other)
	:Super(Other)
	, BTree(Other.BTree)
{

}

AIController::~AIController()
{
	StopTree();
}

void AIController::StartTree()
{
	if(BTree)
	{
		Blackboard* bb = BTree->GetBlackBoard();		

		if(bb)
		{
			BTreeInst = BTree->Clone();		
			BlackBoardInst = bb->Clone();
			BTreeInst->SetBlackboard(BlackBoardInst);

			BTreeInst->SetOwner(this);			
			BlackBoardInst->SetOwner(this);

		}
	}
}

void AIController::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (m_Character && m_Character->IsValidate())
	{
		if (BTreeInst)
		{
			BTreeInst->Update(DeltaSeconds);
		}
	}
	else
		Destroy();
}

void AIController::StopTree()
{
	SAFE_DELETE(BTreeInst);
	SAFE_DELETE(BlackBoardInst);

}

void AIController::BeginPlay()
{
	Super::BeginPlay();	
}

void AIController::OnPossess(class Character* InCharacter)
{
	Super::OnPossess(InCharacter);
	StartTree();	
}

bool AIController::SetBlackboardValueAsBool(const std::string& InKeyName, bool Value)
{
	if(BlackBoardInst)
	{
		return BlackBoardInst->SetBool(InKeyName, Value);
	}

	return false;
}

bool AIController::SetBlackboardValueAsInt(const std::string& InKeyName, int32 Value)
{
	if (BlackBoardInst)
	{
		BlackBoardInst->SetInt(InKeyName, Value);
		return true;
	}

	return false;
}

bool AIController::SetBlackboardValueAsFloat(const std::string& InKeyName, float Value)
{
	if (BlackBoardInst)
	{
		BlackBoardInst->SetFloat(InKeyName, Value);
		return true;
	}

	return false;
}

bool AIController::SetBlackboardValueAsVector(const std::string& InKeyName, const SVector3& Value)
{
	if (BlackBoardInst)
	{
		BlackBoardInst->SetVector(InKeyName, Value);
		return true;
	}

	return false;
}

bool AIController::SetBlackboardValueAsObject(const std::string& InKeyName, XObject* Value)
{
	if (BlackBoardInst)
	{
		BlackBoardInst->SetObj(InKeyName, Value);
		return true;
	}

	return false;
}

bool AIController::SetBlackboardValueAsAbility(const std::string& InKeyName, TSubclassOf<GameplayAbility> Value)
{
	if (BlackBoardInst)
	{
		BlackBoardInst->SetAbility(InKeyName, Value);
		return true;
	}

	return false;
}

bool AIController::RemoveBlackboardValue(const std::string& InKeyName, EBlackBoardValType valueType)
{
	if (BlackBoardInst)
	{
		BlackBoardInst->RemoveKey(InKeyName, valueType);
		return true;
	}

	return false;
}
