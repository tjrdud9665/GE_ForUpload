#include "EnginePCH.h"
#include "BehaviorTree.h"
#include <GameFramework/GameObject/AIController.h>

BehaviorTree::BehaviorTree()
{

}
BehaviorTree::BehaviorTree(const BehaviorTree& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{
		m_Root = Other.m_Root->Clone();
		m_Root->SetTree(this);
		m_Root->SortChildNode();

	}	
}

BehaviorTree::~BehaviorTree()
{

}

void BehaviorTree::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
}




void BehaviorTree::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
	ENodeStatus result = m_Root->UpdateNode(DeltaSeconds);
}

size_t BehaviorTree::GetAllNodes(std::vector<BTNode*>& OutNodes)
{
	size_t nodeSize = m_ChildNodes.size();
	OutNodes.resize(nodeSize);

	for(size_t i = 0;i<nodeSize;i++)
	{
		OutNodes[i] = m_ChildNodes[i];
	}

	return nodeSize;

}

void BehaviorTree::AddChildNode(BTNode* InChild)
{	
	if(!FindNodeByName(InChild->GetName()))
	{
		m_ChildNodes.push_back(InChild);
	}
}

class AIController* BehaviorTree::GetAIController() const
{
	return AIOwner;
}

class Character* BehaviorTree::GetControllerCharacter() const
{
	if(!AIOwner)
	{
		return nullptr;
	}


	return AIOwner->GetCharacter();
}

bool BehaviorTree::Serialize(Archive& ar)
{	
	bool bResult = Super::Serialize(ar);

	//일단 모든 노드를 각 타입에 맞게끔 저장 및 로드 를 해서 모든 노드를 다시 스폰위해 클래스 타입을 먼저 직렬화
	

	if(ar.IsLoading())
	{
	
		ar.BeginObject("ChildNodeClass");
		uint32 nodeSize = 0;
		ar.SerializeArraySize(nodeSize);
		std::vector<XClass*> nodeClasses;
		nodeClasses.resize(nodeSize);
		for (uint32 i = 0; i < nodeSize; i++)
		{
			ar.BeginObject(i);				
			ar.Serialize("val", nodeClasses[i]);
			CreateBTNode<BTNode>(nodeClasses[i]);
			ar.EndObject();
		}
		ar.EndObject();

		

		ar.BeginObject("ChildNodeInst");
		ar.SerializeArraySize(nodeSize);
		for (uint32 i = 0; i < nodeSize; i++)
		{
			ar.BeginObject(i);
			m_ChildNodes[i]->Serialize(ar);
			ar.EndObject();
		}
		ar.EndObject();

		std::string rootName;
		ar.BeginObject("Root");
		ar.Serialize("RootNodeName", rootName);
		ar.EndObject();
		m_Root = (BTCompositeNode*)FindNodeByName(rootName);
	}
	else
	{		
		//std::sort(m_ChildNodes.begin(), m_ChildNodes.end(), &BTNode::SortByOrdinal);
		m_Root->SortChildNode();

		ar.BeginObject("ChildNodeClass");
		uint32 nodeSize = (uint32)m_ChildNodes.size();
		ar.SerializeArraySize(nodeSize);
		for (uint32 i = 0; i < nodeSize; i++)
		{
			ar.BeginObject(i);
			XClass* nodeClass = m_ChildNodes[i]->GetClass();
			ar.Serialize("val", nodeClass);
			ar.EndObject();
		}
		ar.EndObject();


		ar.BeginObject("ChildNodeInst");
		ar.SerializeArraySize(nodeSize);
		for (uint32 i = 0; i < nodeSize; i++)
		{
			ar.BeginObject(i);
			m_ChildNodes[i]->Serialize(ar);
			ar.EndObject();
		}
		ar.EndObject();

		std::string rootName = m_Root->GetName();

		ar.BeginObject("Root");
		ar.Serialize("RootNodeName", rootName);
		ar.EndObject();
		
	}
		

	
	return bResult;
}

bool BehaviorTree::PostLoad()
{
	bool bResult = Super::PostLoad();

	if(!m_Root)
	{
		m_Root = CreateBTNode<BTCompositeNode>();
	}
	else
	{
		m_Root->SortChildNode();
	}
	
	return bResult;
}

void BehaviorTree::SetOwner(XObject* InOwner)
{
	Super::SetOwner(InOwner);
	
	if(InOwner->GetClass()->IsChildOf(AIController::StaticClass()) || 
	   InOwner->GetClass()->IsA(AIController::StaticClass()))
	{
		AIOwner = (AIController*)InOwner;
		m_Root->SetOwner(InOwner);
		m_Root->SetTree(this);
	}


}

void BehaviorTree::SetBlackboard(class Blackboard* InBB)
{
	m_BlackBoard = InBB;
}

bool BTCompositeNode::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);
	if (ar.IsLoading())
	{
		uint32 nodeSize = 0;
		std::stringstream ss;
		ss << "ChildBTNodes_" << GetName();
		ar.BeginObject(ss.str());
		ar.SerializeArraySize(nodeSize);
		for (uint32 i = 0; i < nodeSize; i++)
		{
			ar.BeginObject(i);
			std::string nodeName;
			ar.Serialize("val", nodeName);
			AddChild(m_Tree->FindNodeByName(nodeName));
			ar.EndObject();
		}
		ar.EndObject();
	} else
	{
		uint32 nodeSize = (uint32)m_Children.size();
		std::stringstream ss;
		ss << "ChildBTNodes_" << GetName();
		ar.BeginObject(ss.str());
		ar.SerializeArraySize(nodeSize);
		for (uint32 i = 0; i < nodeSize; i++)
		{
			ar.BeginObject(i);
			std::string childName = m_Children[i]->GetName();
			ar.Serialize("val", childName);
			ar.EndObject();
		}
		ar.EndObject();
	}

	return bResult;
}

bool SBlackboardKeySelector::GetBlackBoardValueAsInt(class BehaviorTree* OwnTree, int32& outVal)
{
	return OwnTree->GetBlackBoard()->GetInt(SeletedKeyName, OUT outVal);
}

bool SBlackboardKeySelector::GetBlackBoardValueAsVector(class BehaviorTree* OwnTree, SVector3& outVal)
{
	return OwnTree->GetBlackBoard()->GetVector(SeletedKeyName, OUT outVal);
}

bool SBlackboardKeySelector::GetBlackBoardValueAsFloat(class BehaviorTree* OwnTree, float& outVal)
{
	return OwnTree->GetBlackBoard()->GetFloat(SeletedKeyName, OUT outVal);
}

bool SBlackboardKeySelector::GetBlackBoardValueAsObject(class BehaviorTree* OwnTree, XObject*& outVal)
{
	return OwnTree->GetBlackBoard()->GetObj(SeletedKeyName, OUT outVal);
}

bool SBlackboardKeySelector::GetBlackBoardValueAsAbility(class BehaviorTree* OwnTree, TSubclassOf<GameplayAbility>& outVal)
{
	return OwnTree->GetBlackBoard()->GetAbility(SeletedKeyName, OUT outVal);
}

bool SBlackboardKeySelector::IsSet(class BehaviorTree* OwnTree) const
{
	switch(SeletedKeyType)
	{
	case EBlackBoardValType::Bool:
		return OwnTree->GetBlackBoard()->HasBool(SeletedKeyName);
	case EBlackBoardValType::Float:
		return OwnTree->GetBlackBoard()->HasFloat(SeletedKeyName);
	case EBlackBoardValType::Vector:
		return OwnTree->GetBlackBoard()->HasVector(SeletedKeyName);
	case EBlackBoardValType::Int:
		return OwnTree->GetBlackBoard()->HasInt(SeletedKeyName);
	case EBlackBoardValType::Object:
		return OwnTree->GetBlackBoard()->HasObj(SeletedKeyName);
	case EBlackBoardValType::Ability:
		return OwnTree->GetBlackBoard()->HasAbility(SeletedKeyName);
	}

	return false;
}

bool SBlackboardKeySelector::GetBlackBoardValueAsBool(class BehaviorTree* OwnTree, bool& outVal)
{
	return OwnTree->GetBlackBoard()->GetBool(SeletedKeyName, OUT outVal);
}

void BTNode::SetTree(BehaviorTree* InTree)
{
	m_Tree = InTree;
	if (m_Tree)
	{
		m_Tree->AddChildNode(this);
	}
}

bool BTNode::SortByOrdinal(const BTNode* src, const BTNode* dest)
{
	return src->NodeOrdinal < dest->NodeOrdinal;
}

bool BTDecoratorNode::Serialize(Archive& ar)
{
	{
		bool bResult = Super::Serialize(ar);

		if (ar.IsLoading())
		{
			
			std::stringstream ss;
			ss << "ChildBTNodes_" << GetName();
			ar.BeginObject(ss.str());
			std::string nodeName;
			ar.Serialize("child", nodeName);
			ar.EndObject();
			AddChild(m_Tree->FindNodeByName(nodeName));

		} else
		{			
			std::stringstream ss;
			ss << "ChildBTNodes_" << GetName();
			ar.BeginObject(ss.str());
			std::string childName = m_Child->GetName();
			ar.Serialize("child", childName);
			ar.EndObject();
		}

		return bResult;
	}

}
