#pragma once
#include "EditorWindow.h"
#include "imgui_node_editor.h"
#include "imgui_node_editor_internal.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <filesystem>
#include "Assets/AI/BehaviorTree.h"

namespace ned = ax::NodeEditor;

enum class PinType
{
	Flow
};

enum class PinKind
{
	Output,
	Input
};

enum class NodeType
{
	Tree,
	Comment
};

NLOHMANN_JSON_SERIALIZE_ENUM(NodeType, {
	{NodeType::Tree, "Tree"},
	{NodeType::Comment, "Comment"}
							 })

enum class NodeKind
{
	Root,
	Sequence,
	Selector,
	Task,
	Deco,
};

NLOHMANN_JSON_SERIALIZE_ENUM(NodeKind, {
	{NodeKind::Root, "Root"},
	{NodeKind::Sequence, "Sequence"},
	{NodeKind::Selector, "Selector"},
	{NodeKind::Task, "Task"},
	{NodeKind::Deco, "Deco"}
							 })

	struct Node;

struct Pin
{
	ned::PinId ID;
	std::string Name;
	PinType     Type;
	PinKind     Kind;
	Node* Node;

	Pin(ned::PinId id, const char* name, PinType type) :
		ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
	{}
};

struct Node
{
	ned::NodeId ID;
	std::string Name;
	ImColor Color;
	NodeType Type;
	NodeKind Kind;
	ImVec2 Size;
	int Ordinal;	
	//Json taskData;
	// need to add stuff that will allow to find out what type of node it is : root, sequence, selector, task (for task we need to remember what task it is
	// mb just a json object?
	std::vector<Pin> Inputs;
	std::vector<Pin> Outputs;
	
	BTNode* NodeData;

	Node(ned::NodeId id, const char* name, ImColor color = ImColor(255, 255, 255)) 
		:ID(id), Name(name), Color(color), Type(NodeType::Tree), Size(0, 0), Kind(NodeKind::Selector), Ordinal(-1)
	{
		NodeData = nullptr;
	}

	~Node()
	{
	
	}


};

struct Link
{
	ned::LinkId ID;

	ned::PinId StartPinID;
	ned::PinId EndPinID;

	ImColor Color;

	Link(ned::LinkId id, ned::PinId startPinId, ned::PinId endPinId) :
		ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
	{}
};

//NodeEditorData

struct NodeEditorData
{
	fs::path treeGraphDataPath;
	ned::EditorContext* context;

	std::vector<Node> nodes;
	std::vector<Link> links;


	ned::Config config;

	bool isDirty = false;
	bool shouldRemainOpen = true;

	std::string editorData;

	
	bool createNewNode = false;
	Pin* newNodeLinkPin = nullptr;
	Pin* newLinkPin = nullptr;
	ned::NodeId contextNodeId = 0;
	ned::LinkId contextLinkId = 0;
	ned::PinId  contextPinId = 0;

	uintptr_t nextId = 1;

	~NodeEditorData()
	{
		if (context != nullptr)
		{
			ned::DestroyEditor(context);
		}
	}

};



class NodeEditor :public EditorWindow
{
	CLASS(NodeEditor,EditorWindow)

protected:
	NodeEditor();
	virtual ~NodeEditor();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

public:
	void SetTargetBTree(BehaviorTree* InTargetBTree);


private:
	void ShowNodeDetails(float paneWidth ,float DeltaSeconds);

	void ShowMenu();

	void DrawNodeEditor(NodeEditorData& nodeEditorData);


	void BuildNodes(NodeEditorData& nodeEditorData);

	void BuildNode(Node* node);

	void BuildContextMenu(NodeEditorData& nodeEditorData);

	void DrawNode(NodeEditorData& nodeEditorData , Node& node);


	
	Node& SpawnCompositeNode(NodeEditorData& nodeEditorData,TSubclassOf<BTCompositeNode> compositeNodeClass , ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0);

	Node& SpawnTaskNode(NodeEditorData& nodeEditorData, TSubclassOf<BTTaskNode> TaskClass, ned::NodeId nodeId = 0, ned::PinId inPinId = 0);

	Node& SpawnDecoratorNode(NodeEditorData& nodeEditorData, TSubclassOf<BTDecoratorNode> decoNodeClass, ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0);



	Node& SpawnNodeFromBTree(NodeEditorData& nodeEditorData, BTNode* nodeInst, ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0);



	Node& SpawnRootNode(NodeEditorData& nodeEditorData, BTCompositeNode* RootInst, ned::NodeId nodeId = 0, ned::PinId outPinId = 0);
	Node& SpawnSequenceNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0);
	Node& SpawnSelectorNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0);
	


	void DrawCreateNodeContextMenuPopup(NodeEditorData& nodeEditorData, const ImVec2& newNodePosition);

	void TryCreateNewLink(NodeEditorData& nodeEditorData);

	void TryDeleteNodesAndLinks(NodeEditorData& nodeEditorData);

	void DrawNodeContextMenuPopup(NodeEditorData& nodeEditorData);
	void DrawPinContextMenuPopup(NodeEditorData& nodeEditorData);
	void DrawLinkContextMenuPopup(NodeEditorData& nodeEditorData);


	void UpdateNodeOrdinals(NodeEditorData& data);
	void UpdateNodeOrdinals_Exec(NodeEditorData& data, Node* curNode, int& curOrdinal);

	bool Load(NodeEditorData& data);
	void Save(NodeEditorData& data);

	BTNode* GetTargetNode(const ned::NodeId& NodeID);


private:
	int32 GetNextId(NodeEditorData& data) 
	{ 
		return (int32)data.nextId++;
	}

	

private:
	std::unordered_map<BehaviorTree*, NodeEditorData>	m_NodeEditorDataPerTree;
	//NodeEditorData m_NodeEditorData;

	//Json taskTypes;

	BehaviorTree* m_TargetTree;

	class PropertySet* m_NodeDetails;
};

