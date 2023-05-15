#pragma once

#include "../AssetData.h"
#include "BlackBoard.h"
/*
* https://www.gamedeveloper.com/programming/behavior-trees-for-ai-how-they-work
* BT
*/
enum class ENodeStatus
{
	Invalid,
	Success,
	Failure,
	Aborted,		// finished aborting = failure
	Running,
};

ENUM(EBlackBoardKeyQueryType, IsSet = 0, IsNotSet);



struct SBlackboardKeySelector
{
	SBlackboardKeySelector()
	{

	}

	std::string SeletedKeyName;
	EBlackBoardValType SeletedKeyType;

	//Unique
	std::vector<EBlackBoardValType>		AllowedTypes;	

	bool GetBlackBoardValueAsBool(class BehaviorTree* OwnTree, bool& outVal);
	bool GetBlackBoardValueAsInt(class BehaviorTree* OwnTree, int32& outVal);
	bool GetBlackBoardValueAsVector(class BehaviorTree* OwnTree, SVector3& outVal);
	bool GetBlackBoardValueAsFloat(class BehaviorTree* OwnTree, float& outVal);
	bool GetBlackBoardValueAsObject(class BehaviorTree* OwnTree, XObject*& outVal);
	bool GetBlackBoardValueAsAbility(class BehaviorTree* OwnTree, TSubclassOf<GameplayAbility>& outVal);

	bool IsSet(class BehaviorTree* OwnTree) const;


};


class BTNode : public XObject
{
	CLASS(BTNode, XObject ,Abstract)	
	friend class BehaviorTree;
	friend class NodeEditor;
	friend class AIController;
public:
	using FBTNodeContainer = std::vector<BTNode*>;

	BTNode()
		:m_Status(ENodeStatus::Invalid)
		, m_Parent(nullptr)
		, NodeOrdinal(-1)
	{

	}
	BTNode(const BTNode& Other)
		:Super(Other)
		,m_Status(ENodeStatus::Invalid)
		, m_Parent(nullptr)
		, NodeOrdinal(Other.NodeOrdinal)
	{

	}
	virtual ~BTNode()
	{

	}

	virtual ENodeStatus UpdateNode(float DeltaSeconds)
	{
		return m_Status;
	}
	virtual void InitNode() {}
	virtual void Terminate(ENodeStatus s) {}

	ENodeStatus Tick(float DeltaSeconds)
	{
		if (m_Status != ENodeStatus::Running)
		{
			InitNode();
		}

		m_Status = UpdateNode(DeltaSeconds);

		if (m_Status != ENodeStatus::Running)
		{
			Terminate(m_Status);
		}

		return m_Status;
	}

	bool isSuccess() const { return m_Status == ENodeStatus::Success; }
	bool isFailure() const { return m_Status == ENodeStatus::Failure; }
	bool isRunning() const { return m_Status == ENodeStatus::Running; }
	bool isTerminated() const { return isSuccess() || isFailure(); }

	void reset() { m_Status = ENodeStatus::Invalid; }

	virtual void AddChild(BTNode* InChild)
	{

	}

	virtual void RemoveChild(BTNode* InChild)
	{


	}

	virtual bool HasChildren() const
	{
		return false;
	}

	virtual void GetChildren(std::vector<BTNode*>& childs)
	{

	}

	//해당 노드가 자식이 될 수 있는지 
	//자식이 될 녀석의 자식중에 내가 있으면 안된다.	
	virtual bool CanBeChild(BTNode* willBeParent)
	{
		return true;
	}

	void SetParent(BTNode* parent)
	{
		m_Parent = parent;
		if(m_Parent)
			m_Tree = m_Parent->m_Tree;
	}
	
	class BehaviorTree* GetBTree() const
	{
		return m_Tree;
	}

	virtual void SetTree(BehaviorTree* InTree);

	virtual void SortChildNode()
	{

	}


	static bool SortByOrdinal(const BTNode* src, const BTNode* dest);

protected:
	ENodeStatus m_Status;	

	BTNode* m_Parent;

	BehaviorTree* m_Tree;

	PROP(int32, NodeOrdinal,DetailsView)
	int32 NodeOrdinal;
};

class BTTaskNode : public BTNode
{
	CLASS (BTTaskNode,BTNode)
public:	

	BTTaskNode()	
	{

	}
	BTTaskNode(const BTTaskNode& Other)
		:Super(Other)		
	{

	}
	virtual ~BTTaskNode()
	{

	}
	virtual void InitNode()  override
	{
		m_Status = ENodeStatus::Running;
	}

};

class BTTaskNode_Test : public BTTaskNode
{
	CLASS(BTTaskNode_Test, BTTaskNode)

public:
	BTTaskNode_Test()
	{
		testBB.AllowedTypes.push_back(EBlackBoardValType::Vector);

	}
	BTTaskNode_Test(const BTTaskNode_Test& Other)
		:Super(Other)
		,testBB(Other.testBB)
	{

	}
	virtual ~BTTaskNode_Test()
	{

	}

	virtual ENodeStatus UpdateNode(float DeltaSeconds)
	{
		//test 로 이곳에서 BB 값을 이용해서 이동을 한다.
		m_Status = ENodeStatus::Success;		


		return m_Status;
	}

	PROP(SBlackboardKeySelector, testBB, DetailsEdit)
	SBlackboardKeySelector testBB;
};

/*
			┏━━━┓
			┃Compo ┃
			┗━━━┛
┏━━━┓ ┏━━━┓┏━━━┓
┃ Node ┃ ┃ Node ┃┃ Node ┃
┗━━━┛ ┗━━━┛┗━━━┛
*/
//복합 노드
//상호 자식관계 방지

class BTCompositeNode : public BTNode
{
	CLASS(BTCompositeNode, BTNode, Abstract)
public:
	BTCompositeNode()
	{

	}
	BTCompositeNode(const BTCompositeNode& Other)
		:Super(Other)
	{

		size_t	Size = Other.m_Children.size();

		for (size_t i = 0; i < Size; ++i)
		{
			BTNode* childNode = Other.m_Children[i]->Clone();

			AddChild(childNode);
		}
	}

	virtual ~BTCompositeNode() 
	{

	}
	bool FindChild(BTNode* InChild)
	{
		size_t childSize = m_Children.size();

		for(size_t i =0;i<childSize;i++)
		{
			if (m_Children[i] == InChild)
				return true;
		}

		return false;
	}

	virtual void AddChild(BTNode* InChild)
	{
		if (InChild)
		{
			if (InChild->CanBeChild(this) && !FindChild(InChild))
			{				
				m_Children.push_back(InChild);
				InChild->SetParent(this);
			}
		}
	}
	virtual bool HasChildren() const
	{
		return !m_Children.empty();
	}

	virtual void GetChildren(std::vector<BTNode*>& childs)
	{
		childs = m_Children;
	}
	//해당 노드가 자식이 될 수 있는지 
	//자식이 될 녀석의 자식중에 내가 있으면 안된다.	
	virtual bool CanBeChild(BTNode* willBeParent)
	{
	

		size_t childSize = m_Children.size();

		for (size_t i = 0; i < childSize; i++)
		{
			if (m_Children[i] == willBeParent)
			{
				return false;
			}
		}

		return true;
	}

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override
	{
		if (!HasChildren())
		{
			return ENodeStatus::Invalid;
		}

		ENodeStatus result;
		FBTNodeContainer::iterator iter = m_Children.begin();
		FBTNodeContainer::iterator iterEnd = m_Children.end();
		for (; iter != iterEnd; iter++)
		{
			result= (*iter)->Tick(DeltaSeconds);
		}
		return result;
	}


	virtual void RemoveChild(BTNode* InChild)
	{
		FBTNodeContainer::iterator iter = m_Children.begin();
		FBTNodeContainer::iterator iterEnd = m_Children.end();

		for (; iter != iterEnd; iter++)
		{
			if (InChild == (*iter))
			{
				(*iter)->SetParent(nullptr);
				m_Children.erase(iter);
				break;
			}
		}
	}

	virtual bool PostLoad() 
	{ 
		bool bResult = Super::PostLoad();		

		return bResult;
	}
	virtual bool Serialize(Archive& ar);

	virtual void SetTree(BehaviorTree* InTree) override
	{
		Super::SetTree(InTree);
		FBTNodeContainer::iterator iter = m_Children.begin();
		FBTNodeContainer::iterator iterEnd = m_Children.end();
		for (; iter != iterEnd; iter++)
		{
			(*iter)->SetTree(InTree);
		}		
	}

	virtual void SortChildNode()
	{
		std::sort(m_Children.begin(), m_Children.end(),&BTNode::SortByOrdinal);
		
		size_t childSize = m_Children.size();

		for(size_t i =0;i<childSize;i++)
		{
			m_Children[i]->SortChildNode();
		}
		

	}

protected:
	FBTNodeContainer m_Children;
};

/*
* 자식의 결과를 부모에게 조작 및 총합하여 전달 . EX) Invertor 는 결과반전
* ->  Single Child.
* 독립 노드라기보다는 노드를 감싸서 결과를 조작하는 컨셉이다
*/

class BTDecoratorNode : public BTNode
{
	CLASS(BTDecoratorNode, BTNode)
public:
	BTDecoratorNode()
		:m_Child(nullptr)
	{
	}
	BTDecoratorNode(const BTDecoratorNode& Other)
		:Super(Other)		
	{
		if(Other.m_Child)
			m_Child = Other.m_Child->Clone();
	}
	virtual ~BTDecoratorNode() {}

	void SetChild(BTNode* InNode)
	{
		m_Child = InNode;
	}
	bool HasChild() const
	{
		return m_Child != nullptr;
	}
	virtual bool HasChildren() const override
	{
		return HasChild();
	}

	virtual void AddChild(BTNode* InChild) override
	{
		SetChild(InChild);
	}
	virtual void RemoveChild(BTNode* InChild)
	{
		if(InChild == m_Child)
			SetChild(nullptr);
	}

	virtual void SetTree(BehaviorTree* InTree) override
	{
		Super::SetTree(InTree);
		m_Child->SetTree(InTree);
	}

	virtual bool Serialize(Archive& ar) override;

	virtual void SortChildNode()
	{
		if (m_Child)
			m_Child->SortChildNode();
	}
	

protected:
	BTNode* m_Child;
};


class BTDecoratorNode_BlackBoardCondition : public BTDecoratorNode
{
	CLASS(BTDecoratorNode_BlackBoardCondition, BTDecoratorNode)

public:
	BTDecoratorNode_BlackBoardCondition() 		
	{
		BlackboardKey.AllowedTypes.push_back(EBlackBoardValType::Bool);
		BlackboardKey.AllowedTypes.push_back(EBlackBoardValType::Float);
		BlackboardKey.AllowedTypes.push_back(EBlackBoardValType::Int);
		BlackboardKey.AllowedTypes.push_back(EBlackBoardValType::Vector);
		BlackboardKey.AllowedTypes.push_back(EBlackBoardValType::Object);
		BlackboardKey.AllowedTypes.push_back(EBlackBoardValType::Ability);
	}
	BTDecoratorNode_BlackBoardCondition(const BTDecoratorNode_BlackBoardCondition& Other)
		:Super(Other)
		,BlackboardKey(Other.BlackboardKey)
		, KeyQuery(Other.KeyQuery)
	{

	}
	virtual ~BTDecoratorNode_BlackBoardCondition() 
	{

	}	

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override
	{
		if (!HasChildren())
		{
			return ENodeStatus::Invalid;
		}	

		if (KeyQuery.Get() == EBlackBoardKeyQueryType::IsSet)
		{
			if (BlackboardKey.IsSet(m_Tree))
			{
				m_Status = m_Child->UpdateNode(DeltaSeconds);
			} else
				m_Status = ENodeStatus::Failure;
		}		
		else
		{
			if (!BlackboardKey.IsSet(m_Tree))
			{
				m_Status = m_Child->UpdateNode(DeltaSeconds);
			}
			else
				m_Status = ENodeStatus::Failure;
		}
			
		return m_Status;
	}



protected:
	PROP(SBlackboardKeySelector, BlackboardKey, DetailsEdit)
	SBlackboardKeySelector BlackboardKey;


	PROP(TEnum<EBlackBoardKeyQueryType>, KeyQuery, DetailsEdit)
	TEnum<EBlackBoardKeyQueryType> KeyQuery;
	

};



// 모든 자식노드들을 InOrder로  순회
// 자식중 하나라도 실패할경우 실패이며 실패한후 더이상 진행되지않음
// 모든 자식노드가 성공할경우 sequence는 성공합니다.
class BTComposite_Sequence : public BTCompositeNode
{
	CLASS(BTComposite_Sequence, BTCompositeNode)
public:
	virtual void InitNode() override
	{

	}

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override
	{
		if (!HasChildren())
		{
			return ENodeStatus::Invalid;
		}

		FBTNodeContainer::iterator iter = m_Children.begin();
		FBTNodeContainer::iterator iterEnd = m_Children.end();
		for (; iter != iterEnd; iter++)
		{
			ENodeStatus status = (*iter)->Tick(DeltaSeconds);
			if (status != ENodeStatus::Success)
			{
				return status;
			}

		}
		return ENodeStatus::Success;
	}

	virtual bool Serialize(Archive& ar) override
	{
		bool bResult = Super::Serialize(ar);

		return bResult;
	}

};

/// <summary>
/// 실패하면 다음 노드를 실행하고 
/// 성공하면 더이상 진행하지 않습니다.
/// </summary>
class BTComposite_Selector : public BTCompositeNode
{
	CLASS(BTComposite_Selector, BTCompositeNode)
public:
	virtual void InitNode() override
	{

	}

	virtual ENodeStatus UpdateNode(float DeltaSeconds) override
	{
		if (!HasChildren())
		{
			return ENodeStatus::Invalid;
		}

		FBTNodeContainer::iterator iter = m_Children.begin();
		FBTNodeContainer::iterator iterEnd = m_Children.end();
		for (; iter != iterEnd; iter++)
		{
			ENodeStatus status = (*iter)->Tick(DeltaSeconds);
			if (status != ENodeStatus::Failure)
			{
				return status;
			}

		}
		return ENodeStatus::Failure;
	}


};


class BehaviorTree : public AssetData
{
	CLASS(BehaviorTree, AssetData)
	friend class AIController;
protected:
	BehaviorTree();
	BehaviorTree(const BehaviorTree& Other);
	virtual ~BehaviorTree();

	virtual void Construct(const std::string& InName = "") override;

	virtual bool Serialize(Archive& ar) override;

	virtual bool PostLoad() override;

	virtual void SetOwner(XObject* InOwner) override;

	void SetBlackboard(class Blackboard* InBB);

public:
	virtual void Update(float DeltaSeconds) override;


public:
	class Blackboard* GetBlackBoard()
	{
		return m_BlackBoard;
	}

	BTNode* FindNodeByName(const std::string& InName)
	{
		size_t nodeSize = m_ChildNodes.size();
		for(size_t i =0;i<nodeSize;i++)
		{
			if (m_ChildNodes[i]->GetName() == InName)
				return m_ChildNodes[i];
		}

		return nullptr;
	}

public:
	template<typename BTNodeType, typename = std::enable_if_t<std::is_base_of<BTNode, BTNodeType>::value>>
	BTNodeType* CreateBTNode(TSubclassOf<BTNode> nodeClass = BTNodeType::StaticClass() , const std::string& NodeName ="")
	{		
		std::string nodeName = NodeName;
		if(nodeName.empty())
			nodeName = nodeClass->GetName();
		stringstream ss;

		auto iter = m_InstanceCounter.find(nodeName);

		if (iter == m_InstanceCounter.end())
			m_InstanceCounter.insert(std::make_pair(nodeName, 0));

		ss << nodeName << "_" << m_InstanceCounter[nodeName]++;

		BTNode* newNode = Globals::CreateX<BTNode>(nodeClass, this, ss.str());
		newNode->m_Tree = this;

		m_ChildNodes.push_back(newNode);
		return (BTNodeType*)newNode;
	}

	void DeleteNode(BTNode* InDeleteNode)
	{
		size_t nodeSize = m_ChildNodes.size();
		for(size_t i =0;i<nodeSize;i++)
		{
			if(m_ChildNodes[i] == InDeleteNode)
			{
				if(m_ChildNodes[i]->m_Parent)
				{
					m_ChildNodes[i]->m_Parent->RemoveChild(InDeleteNode);
				}
				std::vector<BTNode*> childs;
				m_ChildNodes[i]->GetChildren(OUT childs);

				size_t childSize = childs.size();
				for(size_t c= 0;c<childSize;c++)
				{
					childs[c]->SetParent(nullptr);
				}

				m_ChildNodes.erase(m_ChildNodes.begin() + i);
				return;
			}
		}
	}

public:
	BTCompositeNode* GetRoot() const
	{
		return m_Root;
	}

	size_t GetAllNodes(std::vector<BTNode*>& OutNodes);


	//Call Only BTNode
	void AddChildNode(BTNode* InChild);

	class AIController* GetAIController() const;

	class Character* GetControllerCharacter() const;


private:
	BTCompositeNode* m_Root;

	PROP(class Blackboard*, m_BlackBoard, DetailsEdit)
		class Blackboard* m_BlackBoard;


	
	std::vector<TSharedPtr<BTNode>>		m_ChildNodes;

	std::unordered_map<std::string, uint64>	m_InstanceCounter;

	class AIController* AIOwner;

};

