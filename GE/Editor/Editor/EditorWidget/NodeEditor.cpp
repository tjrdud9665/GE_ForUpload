#include "PCH.h"
#include "NodeEditor.h"
#include "Assets/AI/BehaviorTree.h"
#include "PropertySet.h"

static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

static bool CanCreateLink(NodeEditorData& data, Pin* a, Pin* b)
{
	if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
		return false;

	return true;
}

Node* FindNode(NodeEditorData& data, ned::NodeId id)
{
	for (auto& node : data.nodes)
		if (node.ID == id)
			return &node;

	return nullptr;
}

Node* FindRootNode(NodeEditorData& data)
{
	for (auto& node : data.nodes)
		if (node.Kind == NodeKind::Root)
			return &node;

	return nullptr;
}


Pin* FindPin(NodeEditorData& data, ned::PinId id)
{
	if (!id)
		return nullptr;

	for (Node& node : data.nodes)
	{
		for (auto& pin : node.Inputs)
			if (pin.ID == id)
				return &pin;

		for (auto& pin : node.Outputs)
			if (pin.ID == id)
				return &pin;
	}

	return nullptr;
}

Node* FindNodeFromPin(NodeEditorData& data, ned::PinId id)
{
	if (!id)
		return nullptr;

	for (Node& node : data.nodes)
	{
		for (auto& pin : node.Inputs)
			if (pin.ID == id)
				return &node;

		for (auto& pin : node.Outputs)
			if (pin.ID == id)
				return &node;
	}

	return nullptr;
}


Link* FindLink(NodeEditorData& data, ned::LinkId id)
{
	for (auto& link : data.links)
		if (link.ID == id)
			return &link;

	return nullptr;
}

std::vector<Link*> FindLinksByStartPinId(NodeEditorData& data, ned::PinId starPinId)
{
	std::vector<Link*> links;
	for (auto& link : data.links)
		if (link.StartPinID == starPinId)
			links.push_back(&link);

	return links;
}




ImColor GetIconColor(PinType type)
{
	switch (type)
	{
	default:
	case PinType::Flow:     return ImColor(255, 255, 255);
	}
};


NodeEditor::NodeEditor()
{

}

NodeEditor::~NodeEditor()
{

}


void NodeEditor::Build()
{
	Super::Build();

	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoDocking);
	AddFlag(ImGuiWindowFlags_NoCollapse);


	m_NodeDetails = EditorWidget::AddNew<PropertySet>("NodeProperties");
	

}

void NodeEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{		
		BeginWindow();
		ImVec2 winSize = ImGui::GetWindowSize();
		winSize.x = std::max(winSize.x, 250.0f);
		winSize.y = std::max(winSize.y, 250.0f);
		ImGui::SetWindowSize(GetName().c_str(), winSize);

		ShowMenu();
		static float leftPaneWidth = 400.0f;
		static float rightPaneWidth = 800.0f;
		Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);
		ShowNodeDetails(leftPaneWidth - 4.0f,DeltaSeconds);
		ImGui::SameLine(0.0f, 12.0f);
		if(m_TargetTree)
			DrawNodeEditor(m_NodeEditorDataPerTree[m_TargetTree]);
	

		EndWindow();
	}
}

void NodeEditor::SetTargetBTree(BehaviorTree* InTargetBTree)
{
	m_TargetTree = InTargetBTree;

	
	if(m_TargetTree)
	{	
		std::unordered_map<BehaviorTree*, NodeEditorData>::iterator find_iter = m_NodeEditorDataPerTree.find(m_TargetTree);		
		
		if(find_iter == m_NodeEditorDataPerTree.end())
		{
			m_NodeEditorDataPerTree.insert(std::make_pair(m_TargetTree,NodeEditorData()));
			m_NodeEditorDataPerTree[m_TargetTree].context = ned::CreateEditor(&m_NodeEditorDataPerTree[m_TargetTree].config);

			fs::path btGraphPath = m_TargetTree->GetRawPath();
			m_NodeEditorDataPerTree[m_TargetTree].treeGraphDataPath = btGraphPath.replace_extension(".grph");

			if (!Load(m_NodeEditorDataPerTree[m_TargetTree]))
			{
				SpawnRootNode(m_NodeEditorDataPerTree[m_TargetTree], m_TargetTree->GetRoot());
				
			}

			//std::vector<BTNode*> allNodes;
			//size_t nodeCnt = m_TargetTree->GetAllNodes(OUT allNodes);

			//for (size_t i = 0; i < nodeCnt; i++)
			//{
			//	//root 
			//	if (allNodes[i] == m_TargetTree->GetRoot())
			//	{
			//		//Root Node
			//		SpawnRootNode(m_NodeEditorDataPerTree[m_TargetTree], m_TargetTree->GetRoot());
			//	}

			//}
		}		
	
		ned::SetCurrentEditor(m_NodeEditorDataPerTree[m_TargetTree].context);		

	

	}
}

void NodeEditor::ShowNodeDetails(float paneWidth, float DeltaSeconds)
{
	ImGui::BeginChild("NodeDetails", ImVec2(paneWidth, 0));

	paneWidth = ImGui::GetContentRegionAvail().x;
	std::vector<ned::NodeId> selectedNodes;
	selectedNodes.resize(ned::GetSelectedObjectCount());
	int nodeCount = ned::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
	
	if(nodeCount >0)
	{
		BTNode* targetNode = GetTargetNode(selectedNodes[0]);		
		if(m_NodeDetails->GetPropertyTarget() != targetNode)
		{
			m_NodeDetails->SetPropertyTarget(targetNode);
		}		
	}
	else
	{
		if (m_NodeDetails->GetPropertyTarget() != m_TargetTree)
			m_NodeDetails->SetPropertyTarget(m_TargetTree);

	}
	m_NodeDetails->OnUpdate(DeltaSeconds);

	ImGui::EndChild();
}

void NodeEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_TargetTree)
				{
					m_TargetTree->Save();
					fs::path btGraphPath = m_TargetTree->GetRawPath();					
					m_NodeEditorDataPerTree[m_TargetTree].treeGraphDataPath = btGraphPath.replace_extension(".grph");
					Save(m_NodeEditorDataPerTree[m_TargetTree]);
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void NodeEditor::DrawNodeEditor(NodeEditorData& nodeEditorData)
{	

	//ned::SetCurrentEditor(nodeEditorData.context);
	ned::Begin("BT Edtior");

	auto cursorTopLeft = ImGui::GetCursorScreenPos();

	// draw this editor's nodes
	for (Node& node : nodeEditorData.nodes)
	{
		DrawNode(nodeEditorData, node);
	}

	// draw links
	for (Link& link : nodeEditorData.links)
	{
		ned::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
	}

	if (!nodeEditorData.createNewNode)
	{
		// draw stuff to create new links
		TryCreateNewLink(nodeEditorData);

		// draw stuff to delete
		TryDeleteNodesAndLinks(nodeEditorData);
	}

	ImGui::SetCursorScreenPos(cursorTopLeft);


	auto openPopupPosition = ImGui::GetMousePos();
	ned::Suspend();
	// poll context menus
	if (ned::ShowNodeContextMenu(&nodeEditorData.contextNodeId))
		ImGui::OpenPopup("Node Context Menu");
	else if (ned::ShowPinContextMenu(&nodeEditorData.contextPinId))
		ImGui::OpenPopup("Pin Context Menu");
	else if (ned::ShowLinkContextMenu(&nodeEditorData.contextLinkId))
		ImGui::OpenPopup("Link Context Menu");
	else if (ned::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		nodeEditorData.newNodeLinkPin = nullptr;
	}
	ned::Resume();

	ned::Suspend();
	// draw opened context menus
	DrawNodeContextMenuPopup(nodeEditorData);
	DrawPinContextMenuPopup(nodeEditorData);
	DrawLinkContextMenuPopup(nodeEditorData);
	DrawCreateNodeContextMenuPopup(nodeEditorData, openPopupPosition);
	ned::Resume();

	ned::End();

	auto editorMin = ImGui::GetItemRectMin();
	auto editorMax = ImGui::GetItemRectMax();

	if (true)
	{
		UpdateNodeOrdinals(nodeEditorData);

		auto drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(editorMin, editorMax);

		for (auto& node : nodeEditorData.nodes)
		{
			if (node.Kind == NodeKind::Root)
			{
				continue;
			}
			ned::NodeId nodeId = node.ID;
			auto p0 = ned::GetNodePosition(nodeId);
			auto p1 = p0 + ned::GetNodeSize(nodeId);
			p0 = ned::CanvasToScreen(p0);
			p1 = ned::CanvasToScreen(p1);


			ImGuiTextBuffer builder;
			builder.appendf("#%d", node.Ordinal);

			auto textSize = ImGui::CalcTextSize(builder.c_str());
			auto padding = ImVec2(2.0f, 2.0f);
			auto widgetSize = textSize + padding * 2;

			auto widgetPosition = ImVec2(p1.x, p0.y) + ImVec2(0.0f, -widgetSize.y);

			drawList->AddRectFilled(widgetPosition, widgetPosition + widgetSize, IM_COL32(100, 80, 80, 190), 3.0f, ImDrawFlags_RoundCornersAll);
			drawList->AddRect(widgetPosition, widgetPosition + widgetSize, IM_COL32(200, 160, 160, 190), 3.0f, ImDrawFlags_RoundCornersAll);
			drawList->AddText(widgetPosition + padding, IM_COL32(255, 255, 255, 255), builder.c_str());
		}

		drawList->PopClipRect();
	}
	

}

void NodeEditor::BuildNodes(NodeEditorData& nodeEditorData)
{
	size_t nodeCnt = nodeEditorData.nodes.size();
	for (size_t i =0;i<nodeCnt;i++)
		BuildNode(&nodeEditorData.nodes[i]);		
}

void NodeEditor::BuildNode(Node* node)
{
	for (auto& input : node->Inputs)
	{
		input.Node = node;
		input.Kind = PinKind::Input;
	}

	for (auto& output : node->Outputs)
	{
		output.Node = node;
		output.Kind = PinKind::Output;
	}
}


void NodeEditor::BuildContextMenu(NodeEditorData& nodeEditorData)
{
	auto openPopupPosition = ImGui::GetMousePos();

	ned::Suspend();	

	if (ned::ShowNodeContextMenu(&nodeEditorData.contextNodeId))
		ImGui::OpenPopup("Node Context Menu");
	else if (ned::ShowPinContextMenu(&nodeEditorData.contextPinId))
		ImGui::OpenPopup("Pin Context Menu");
	else if (ned::ShowLinkContextMenu(&nodeEditorData.contextLinkId))
		ImGui::OpenPopup("Link Context Menu");
	else if (ned::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		nodeEditorData.newNodeLinkPin = nullptr;
	}
	ned::Resume();

	ned::Suspend();
	DrawNodeContextMenuPopup(nodeEditorData);
	DrawPinContextMenuPopup(nodeEditorData);
	DrawLinkContextMenuPopup(nodeEditorData);
	DrawCreateNodeContextMenuPopup(nodeEditorData, openPopupPosition);

	ned::Resume();
	
}
	

void NodeEditor::DrawNode(NodeEditorData& nodeEditorData, Node& node)
{
	if (node.Type == NodeType::Tree)
	{
		const float rounding = 5.0f;
		const float padding = 12.0f;

		const auto pinBackground = ned::GetStyle().Colors[ned::StyleColor_NodeBg];

		ned::PushStyleColor(ned::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
		ned::PushStyleColor(ned::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
		ned::PushStyleColor(ned::StyleColor_PinRect, ImColor(60, 180, 255, 150));
		ned::PushStyleColor(ned::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

		ned::PushStyleVar(ned::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
		ned::PushStyleVar(ned::StyleVar_NodeRounding, rounding);
		ned::PushStyleVar(ned::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
		ned::PushStyleVar(ned::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
		ned::PushStyleVar(ned::StyleVar_LinkStrength, 0.0f);
		ned::PushStyleVar(ned::StyleVar_PinBorderWidth, 1.0f);
		ned::PushStyleVar(ned::StyleVar_PinRadius, 5.0f);
		ned::BeginNode(node.ID);

		ImGui::BeginVertical(node.ID.AsPointer());
		ImGui::BeginHorizontal("inputs");
		ImGui::Spring(0, padding * 2);

		ImRect inputsRect;
		int inputAlpha = 200;
		if (!node.Inputs.empty())
		{
			auto& pin = node.Inputs[0];
			ImGui::Dummy(ImVec2(0, padding));
			ImGui::Spring(1, 0);
			inputsRect = ned::Detail::ImGui_GetItemRect();

			ned::PushStyleVar(ned::StyleVar_PinArrowSize, 10.0f);
			ned::PushStyleVar(ned::StyleVar_PinArrowWidth, 10.0f);

			ned::PushStyleVar(ned::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);

			ned::BeginPin(pin.ID, ned::PinKind::Input);
			ned::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
			ned::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
			ned::EndPin();
			ned::PopStyleVar(3);

			if (nodeEditorData.newLinkPin && !CanCreateLink(nodeEditorData, nodeEditorData.newLinkPin, &pin) && &pin != nodeEditorData.newLinkPin)
				inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
		} else
		{
			ImGui::Dummy(ImVec2(0, padding));
		}

		ImGui::Spring(0, padding * 2);
		ImGui::EndHorizontal();

		ImGui::BeginHorizontal("content_frame");
		ImGui::Spring(1, padding);

		ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
		ImGui::Dummy(ImVec2(160, 0));
		ImGui::Spring(1);
		ImGui::TextUnformatted(node.Name.c_str());
		ImGui::Spring(1);

		ImGui::EndVertical();
		auto contentRect = ned::Detail::ImGui_GetItemRect();
		

		ImGui::Spring(1, padding);
		ImGui::EndHorizontal();

		ImGui::BeginHorizontal("outputs");
		ImGui::Spring(0, padding * 2);

		ImRect outputsRect;
		int outputAlpha = 200;
		if (!node.Outputs.empty())
		{
			auto& pin = node.Outputs[0];
			ImGui::Dummy(ImVec2(0, padding));
			ImGui::Spring(1, 0);
			outputsRect = ned::Detail::ImGui_GetItemRect();

			ned::PushStyleVar(ned::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);

			ned::BeginPin(pin.ID, ned::PinKind::Output);
			ned::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
			ned::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
			ned::EndPin();
			ned::PopStyleVar();

			if (nodeEditorData.newLinkPin && !CanCreateLink(nodeEditorData, nodeEditorData.newLinkPin, &pin) && &pin != nodeEditorData.newLinkPin)
				outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
		} else
			ImGui::Dummy(ImVec2(0, padding));

		ImGui::Spring(0, padding * 2);
		ImGui::EndHorizontal();

		ImGui::EndVertical();

		ned::EndNode();
		ned::PopStyleVar(7);
		ned::PopStyleColor(4);

		auto drawList = ned::GetNodeBackgroundDrawList(node.ID);


		const auto    topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
		const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;


		if (!node.Inputs.empty())
		{
			drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
									IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
							  IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
			//ImGui::PopStyleVar();
		}
		if (!node.Outputs.empty())
		{
			drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
									IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
							  IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
			//ImGui::PopStyleVar();
		}
		drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
		drawList->AddRect(
			contentRect.GetTL(),
			contentRect.GetBR(),
			IM_COL32(48, 128, 255, 100), 0.0f);	

	}
	else
	{
		
	}
	
}


Node& NodeEditor::SpawnCompositeNode(NodeEditorData& nodeEditorData, TSubclassOf<BTCompositeNode> compositeNodeClass, ned::NodeId nodeId /*= 0*/, ned::PinId inPinId /*= 0*/, ned::PinId outPinId /*= 0*/)
{
	if (nodeId.Get() == 0)
		nodeId = GetNextId(nodeEditorData);
	if (inPinId.Get() == 0)
		inPinId = GetNextId(nodeEditorData);
	if (outPinId.Get() == 0)
		outPinId = GetNextId(nodeEditorData);

	Node* pNode = nullptr;

	if (compositeNodeClass->IsA(BTComposite_Sequence::StaticClass()))
	{
		Node& node = nodeEditorData.nodes.emplace_back(nodeId, "Sequence");
		node.Type = NodeType::Tree;
		node.Inputs.emplace_back(inPinId, "", PinType::Flow);
		node.Outputs.emplace_back(outPinId, "", PinType::Flow);
		node.Kind = NodeKind::Sequence;		
		node.NodeData = m_TargetTree->CreateBTNode<BTComposite_Sequence>(compositeNodeClass);
		pNode = &node;

	} else if (compositeNodeClass->IsA(BTComposite_Selector::StaticClass()))
	{
		Node& node = nodeEditorData.nodes.emplace_back(nodeId, "Selector");
		node.Type = NodeType::Tree;
		node.Inputs.emplace_back(inPinId, "", PinType::Flow);
		node.Outputs.emplace_back(outPinId, "", PinType::Flow);
		node.Kind = NodeKind::Selector;		
		node.NodeData = m_TargetTree->CreateBTNode<BTComposite_Selector>(compositeNodeClass);
		pNode = &node;
	}

	BuildNode(pNode);
	return *pNode;
}

Node& NodeEditor::SpawnTaskNode(NodeEditorData& nodeEditorData, TSubclassOf<BTTaskNode> TaskClass, ned::NodeId nodeId /*= 0*/, ned::PinId inPinId /*= 0*/)
{
	if (nodeId.Get() == 0) nodeId = GetNextId(nodeEditorData);
	if (inPinId.Get() == 0) inPinId = GetNextId(nodeEditorData);

	Node& node =nodeEditorData.nodes.emplace_back(nodeId, TaskClass.Class->GetName().c_str() , ImColor(255, 0, 255));
	node.Type = NodeType::Tree;
	node.Inputs.emplace_back(inPinId, "", PinType::Flow);
	node.Kind = NodeKind::Task;
	node.NodeData = m_TargetTree->CreateBTNode<BTTaskNode>(TaskClass);
	BuildNode(&node);

	return node;
}

Node& NodeEditor::SpawnDecoratorNode(NodeEditorData& nodeEditorData, TSubclassOf<BTDecoratorNode> decoNodeClass, ned::NodeId nodeId /*= 0*/, ned::PinId inPinId /*= 0*/, ned::PinId outPinId /*= 0*/)
{
	if (nodeId.Get() == 0)
		nodeId = GetNextId(nodeEditorData);
	if (inPinId.Get() == 0)
		inPinId = GetNextId(nodeEditorData);
	if (outPinId.Get() == 0)
		outPinId = GetNextId(nodeEditorData);

	Node* pNode = nullptr;

	
	Node& node = nodeEditorData.nodes.emplace_back(nodeId, decoNodeClass.Class->GetName().c_str());
	node.Type = NodeType::Tree;
	node.Inputs.emplace_back(inPinId, "", PinType::Flow);
	node.Outputs.emplace_back(outPinId, "", PinType::Flow);
	node.Kind = NodeKind::Deco;
	node.NodeData = m_TargetTree->CreateBTNode<BTDecoratorNode>(decoNodeClass);
	pNode = &node;

	

	BuildNode(pNode);
	return *pNode;
}

Node& NodeEditor::SpawnNodeFromBTree(NodeEditorData& nodeEditorData, BTNode* nodeInst, ned::NodeId nodeId /*= 0*/, ned::PinId inPinId /*= 0*/, ned::PinId outPinId /*= 0*/)
{
	if (nodeId.Get() == 0) nodeId = GetNextId(nodeEditorData);
	if (inPinId.Get() == 0) inPinId = GetNextId(nodeEditorData);
	if (outPinId.Get() == 0) outPinId = GetNextId(nodeEditorData);

	std::string nodeTypeName;
	NodeKind Kind;
	if(nodeInst->GetClass()->IsA(BTComposite_Sequence::StaticClass()))
	{
		nodeTypeName = "Sequence";
		Kind = NodeKind::Sequence;
	}
	else if (nodeInst->GetClass()->IsA(BTComposite_Selector::StaticClass()))
	{
		nodeTypeName = "Selector";
		Kind = NodeKind::Selector;
	}
	else if (nodeInst->GetClass()->IsChildOf(BTDecoratorNode::StaticClass()))
	{
		nodeTypeName = nodeInst->GetName();
		Kind = NodeKind::Deco;
	}
	else 
	{
		nodeTypeName = nodeInst->GetName();
		Kind = NodeKind::Task;
	}

	Node& node = nodeEditorData.nodes.emplace_back(nodeId, nodeTypeName.c_str());
	node.Type = NodeType::Tree;
	node.Inputs.emplace_back(inPinId, "", PinType::Flow);
	if(Kind != NodeKind::Task)
		node.Outputs.emplace_back(outPinId, "", PinType::Flow);
	node.Kind = Kind;
	node.NodeData = nodeInst;

	BuildNode(&node);

	return node;
}

Node& NodeEditor::SpawnRootNode(NodeEditorData& nodeEditorData, BTCompositeNode* RootInst, ned::NodeId nodeId /*= 0*/, ned::PinId outPinId /*= 0*/)
{
	if (nodeId.Get() == 0) nodeId = GetNextId(nodeEditorData);
	if (outPinId.Get() == 0) outPinId = GetNextId(nodeEditorData);

	Node& node = nodeEditorData.nodes.emplace_back(nodeId, "Root");
	node.Type = NodeType::Tree;
	node.Outputs.emplace_back(outPinId, "", PinType::Flow);
	node.Kind = NodeKind::Root;	
	node.NodeData = RootInst;
	BuildNode(&node);

	return node;
}

Node& NodeEditor::SpawnSequenceNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId /*= 0*/, ned::PinId inPinId /*= 0*/, ned::PinId outPinId /*= 0*/)
{
	if (nodeId.Get() == 0) nodeId = GetNextId(nodeEditorData);
	if (inPinId.Get() == 0) inPinId = GetNextId(nodeEditorData);
	if (outPinId.Get() == 0) outPinId = GetNextId(nodeEditorData);

	Node& node = nodeEditorData.nodes.emplace_back(nodeId, "Sequence");
	node.Type = NodeType::Tree;
	node.Inputs.emplace_back(inPinId, "", PinType::Flow);
	node.Outputs.emplace_back(outPinId, "", PinType::Flow);
	node.Kind = NodeKind::Sequence;	
	BuildNode(&node);

	return node;
}

Node& NodeEditor::SpawnSelectorNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId /*= 0*/, ned::PinId inPinId /*= 0*/, ned::PinId outPinId /*= 0*/)
{
	if (nodeId.Get() == 0) nodeId = GetNextId(nodeEditorData);
	if (inPinId.Get() == 0) inPinId = GetNextId(nodeEditorData);
	if (outPinId.Get() == 0) outPinId = GetNextId(nodeEditorData);

	Node& node = nodeEditorData.nodes.emplace_back(nodeId, "Selector");
	node.Type = NodeType::Tree;
	node.Inputs.emplace_back(inPinId, "", PinType::Flow);
	node.Outputs.emplace_back(outPinId, "", PinType::Flow);
	node.Kind = NodeKind::Selector;

	BuildNode(&node);

	return node;

}


void NodeEditor::DrawCreateNodeContextMenuPopup(NodeEditorData& nodeEditorData, const ImVec2& newNodePosition)
{
	std::vector<XClass*> compositeNodeClass = BTCompositeNode::StaticClass()->GetOwnChildren();

	std::vector<XClass*> taskNodeClass = BTTaskNode::StaticClass()->GetOwnChildren();

	std::vector<XClass*> decoNodeClass = BTDecoratorNode::StaticClass()->GetOwnChildren();

	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::BeginPopup("Create New Node"))
	{
		Node* node = nullptr;
		if (ImGui::CollapsingHeader("Composites"))
		{
			size_t  compositeNode = compositeNodeClass.size();
			for(size_t i =0;i< compositeNode;i++)
			{				
				if(ImGui::MenuItem(compositeNodeClass[i]->GetName().c_str()))
				{
					node = &SpawnCompositeNode(nodeEditorData, compositeNodeClass[i]);					
				}				
			}
		}
		if (ImGui::CollapsingHeader("Decorators"))
		{
			size_t  decoNodeSize = decoNodeClass.size();
			for (size_t i = 0; i < decoNodeSize; i++)
			{
				if (ImGui::MenuItem(decoNodeClass[i]->GetName().c_str()))
				{
					node = &SpawnDecoratorNode(nodeEditorData, decoNodeClass[i]);
				}
			}
		}
		if (ImGui::CollapsingHeader("Tasks"))
		{
			size_t  taskSize = taskNodeClass.size();
			for (size_t i = 0; i < taskSize; i++)
			{

				if (ImGui::MenuItem(taskNodeClass[i]->GetName().c_str()))
				{
					node = &SpawnTaskNode(nodeEditorData, taskNodeClass[i]);

				}

			}
			
		}

		if (node)
		{
			// we need this because adding a new node to the vector might move the nodes, which will break pointers in pins
			BuildNodes(nodeEditorData);

			nodeEditorData.createNewNode = false;

			ned::SetNodePosition(node->ID, newNodePosition);

			if (auto startPin = nodeEditorData.newNodeLinkPin)
			{
				auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

				for (auto& pin : pins)
				{
					if (CanCreateLink(nodeEditorData, startPin, &pin))
					{
						auto endPin = &pin;
						if (startPin->Kind == PinKind::Input)
							std::swap(startPin, endPin);

						// todo: reuse this code?
						////////////////////////////
						auto iterToExistingEndIdLink = std::find_if(nodeEditorData.links.begin(), nodeEditorData.links.end(), [endPin](const Link& link) { return link.EndPinID == endPin->ID; });
						if (iterToExistingEndIdLink != nodeEditorData.links.end())
						{
							nodeEditorData.links.erase(iterToExistingEndIdLink);
						}
						if (startPin->Node && startPin->Node->Kind == NodeKind::Root)
						{
							auto iterToExistingStartIdLink = std::find_if(nodeEditorData.links.begin(), nodeEditorData.links.end(), [startPin](const Link& link) { return link.StartPinID == startPin->ID; });
							if (iterToExistingStartIdLink != nodeEditorData.links.end()) nodeEditorData.links.erase(iterToExistingStartIdLink);
						}
						////////////////////////////

						Link newLink = nodeEditorData.links.emplace_back(Link(GetNextId(nodeEditorData), startPin->ID, endPin->ID));
						nodeEditorData.isDirty = true;
						Node* startNode = FindNodeFromPin(nodeEditorData, newLink.StartPinID);
						Node* endNode = FindNodeFromPin(nodeEditorData, newLink.EndPinID);
						startNode->NodeData->AddChild(endNode->NodeData);					

						break;
					}
				}
			}
		}

		ImGui::EndPopup();
	} else
	{
		nodeEditorData.createNewNode = false;
	}
}

void NodeEditor::TryCreateNewLink(NodeEditorData& nodeEditorData)
{
	if (ned::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto showLabel = [](const char* label, ImColor color)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

			auto rectMin = ImGui::GetCursorScreenPos() - padding;
			auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		ned::PinId startPinId = 0, endPinId = 0;
		if (ned::QueryNewLink(&startPinId, &endPinId))
		{
			auto startPin = FindPin(nodeEditorData, startPinId);
			auto endPin = FindPin(nodeEditorData, endPinId);

			nodeEditorData.newLinkPin = startPin ? startPin : endPin;

			if (startPin->Kind == PinKind::Input)
			{
				std::swap(startPin, endPin);
				std::swap(startPinId, endPinId);
			}

			// todo: check for graph cycles? - can there be cycles in the tree (if only one input is allowed)?

			if (startPin && endPin)
			{
				auto iterToExistingEndIdLink = std::find_if(nodeEditorData.links.begin(), nodeEditorData.links.end(), [endPin](const Link& link) { return link.EndPinID == endPin->ID; });
				auto iterToExistingStartIdLink = std::find_if(nodeEditorData.links.begin(), nodeEditorData.links.end(), [startPin](const Link& link) { return link.StartPinID == startPin->ID; });

				if (endPin == startPin)
				{
					ned::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				} else if (endPin->Kind == startPin->Kind)
				{
					showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
					ned::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (startPin->Node->Kind == NodeKind::Root && 
						 endPin->Node->Kind == NodeKind::Task)
				{
					showLabel("Root Can't Child To Task", ImColor(45, 32, 32, 180));
					ned::RejectNewItem(ImColor(255, 0, 0), 2.0f);

				}
				else if (endPin->Node == startPin->Node)
				{
					showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
					ned::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				}
				else if (endPin->Type != startPin->Type)
				{
					showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
					ned::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				} 
				else if (startPin->Node->Kind == NodeKind::Task && startPin->Node->NodeData->HasChildren())
				{
					showLabel("Decorator Can't Multiple Child", ImColor(45, 32, 32, 180));
					ned::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else
				{
					if (iterToExistingEndIdLink != nodeEditorData.links.end() || iterToExistingStartIdLink != nodeEditorData.links.end())
					{
						showLabel("+ Replace Link", ImColor(32, 45, 32, 180));
					} else
					{
						showLabel("+ Create Link", ImColor(32, 45, 32, 180));
					}
					if (ned::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						if (iterToExistingEndIdLink != nodeEditorData.links.end())
						{
							nodeEditorData.links.erase(iterToExistingEndIdLink);
						}
						if (startPin->Node && (startPin->Node->Kind == NodeKind::Root || startPin->Node->Kind == NodeKind::Deco))
						{
							iterToExistingStartIdLink = std::find_if(nodeEditorData.links.begin(), nodeEditorData.links.end(), [startPin](const Link& link) { return link.StartPinID == startPin->ID; });

							if (iterToExistingStartIdLink != nodeEditorData.links.end())
							{
								Link* link = FindLink(nodeEditorData, (*iterToExistingStartIdLink).ID);
								Node* startNode	= FindNodeFromPin(nodeEditorData, link->StartPinID);
								Node* endNode = FindNodeFromPin(nodeEditorData, link->EndPinID);

								startNode->NodeData->RemoveChild(endNode->NodeData);

								nodeEditorData.links.erase(iterToExistingStartIdLink);
							}
						}
						if(startPinId != endPinId)
						{
							Link& link = nodeEditorData.links.emplace_back(Link(GetNextId(nodeEditorData), startPinId, endPinId));


							Node* startNode = FindNodeFromPin(nodeEditorData, link.StartPinID);
							Node* endNode = FindNodeFromPin(nodeEditorData, link.EndPinID);

							startNode->NodeData->AddChild(endNode->NodeData);


							nodeEditorData.isDirty = true;
							link.Color = GetIconColor(startPin->Type);
						}
					
					}
				}
			}
		}

		ned::PinId pinId = 0;
		if (ned::QueryNewNode(&pinId))
		{
			nodeEditorData.newLinkPin = FindPin(nodeEditorData, pinId);
			if (nodeEditorData.newLinkPin)
				showLabel("+ Create Node", ImColor(32, 45, 32, 180));

			if (ned::AcceptNewItem())
			{
				nodeEditorData.createNewNode = true;
				nodeEditorData.newNodeLinkPin = FindPin(nodeEditorData, pinId);
				nodeEditorData.newLinkPin = nullptr;
				ned::Suspend();
				ImGui::OpenPopup("Create New Node");
				ned::Resume();
			}
		}
	} else
	{
		nodeEditorData.newLinkPin = nullptr;
	}

	ned::EndCreate();
}

void NodeEditor::TryDeleteNodesAndLinks(NodeEditorData& nodeEditorData)
{
	bool deletedNode = false;
	if (ned::BeginDelete())
	{
		ned::NodeId nodeId = 0;
		while (ned::QueryDeletedNode(&nodeId))
		{
			if (FindNode(nodeEditorData, nodeId)->Kind == NodeKind::Root)
			{
				ned::RejectDeletedItem();
			} else if (ned::AcceptDeletedItem())
			{
				std::vector<Node>& nodes = nodeEditorData.nodes;
				auto iter = std::find_if(nodes.begin(), nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
				if (iter != nodes.end())
				{
					m_TargetTree->DeleteNode((*iter).NodeData);					
					nodes.erase(iter);

				}
				deletedNode = true;
			}
		}

		ned::LinkId linkId = 0;
		while (ned::QueryDeletedLink(&linkId))
		{
			if (ned::AcceptDeletedItem())
			{
				std::vector<Link>& links = nodeEditorData.links;
				auto iter = std::find_if(links.begin(), links.end(), [linkId](auto& link) { return link.ID == linkId; });
				if (iter != links.end())
					links.erase(iter);
			}
		}
	}
	ned::EndDelete();
	if (deletedNode)
	{
		BuildNodes(nodeEditorData);
	}
}

void NodeEditor::DrawNodeContextMenuPopup(NodeEditorData& nodeEditorData)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("Node Context Menu"))
	{
		auto node = FindNode(nodeEditorData, nodeEditorData.contextNodeId);

		ImGui::TextUnformatted("Node Context Menu");
		ImGui::Separator();		

		ImGui::Separator();
		if (ImGui::MenuItem("Delete", nullptr, nullptr, node->Kind != NodeKind::Root))
			ned::DeleteNode(nodeEditorData.contextNodeId);
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void NodeEditor::DrawPinContextMenuPopup(NodeEditorData& nodeEditorData)
{
	if (ImGui::BeginPopup("Pin Context Menu"))
	{
		auto pin = FindPin(nodeEditorData, nodeEditorData.contextPinId);

		ImGui::TextUnformatted("Pin Context Menu");
		ImGui::Separator();
		if (pin)
		{
			ImGui::Text("ID: %p", pin->ID.AsPointer());
			if (pin->Node)
				ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
			else
				ImGui::Text("Node: %s", "<none>");
		} else
			ImGui::Text("Unknown pin: %p", nodeEditorData.contextPinId.AsPointer());

		ImGui::EndPopup();
	}
}

void NodeEditor::DrawLinkContextMenuPopup(NodeEditorData& nodeEditorData)
{
	if (ImGui::BeginPopup("Link Context Menu"))
	{
		auto link = FindLink(nodeEditorData, nodeEditorData.contextLinkId);

		ImGui::TextUnformatted("Link Context Menu");
		ImGui::Separator();
		if (link)
		{
			ImGui::Text("ID: %p", link->ID.AsPointer());
			ImGui::Text("From: %p", link->StartPinID.AsPointer());
			ImGui::Text("To: %p", link->EndPinID.AsPointer());
		} else
			ImGui::Text("Unknown link: %p", nodeEditorData.contextLinkId.AsPointer());
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
		{
			Link* lnk = FindLink(nodeEditorData, nodeEditorData.contextLinkId);
			
			Node* startNode = FindNodeFromPin(nodeEditorData, lnk->StartPinID);
			Node* endNode = FindNodeFromPin(nodeEditorData, lnk->EndPinID);
			startNode->NodeData->RemoveChild(endNode->NodeData);

			ned::DeleteLink(nodeEditorData.contextLinkId);

		}
		ImGui::EndPopup();
	}
}

void NodeEditor::UpdateNodeOrdinals(NodeEditorData& data)
{
	Node* root = FindRootNode(data);

	// reset ordinals (this is for unconnected nodes)
	for (Node& node : data.nodes)
	{
		node.Ordinal = -1;
		node.NodeData->NodeOrdinal = -1;
	}

	assert(root != nullptr && "Root node was not found");

	int curOrdinal = -1;
	UpdateNodeOrdinals_Exec(data, root, curOrdinal);
}

void NodeEditor::UpdateNodeOrdinals_Exec(NodeEditorData& data, Node* curNode, int& curOrdinal)
{
	curNode->Ordinal = curOrdinal++;
	curNode->NodeData->NodeOrdinal = curNode->Ordinal;

	if (curNode->Outputs.size() == 0)
	{
		return;
	}
	std::vector<Link*> outputLinks = FindLinksByStartPinId(data, curNode->Outputs[0].ID);

	std::vector<Node*> directChildNodes;
	directChildNodes.reserve(outputLinks.size());
	for (Link* link : outputLinks)
	{
		Node* node = FindPin(data, link->EndPinID)->Node;
		assert(node != nullptr && "A pin doesn't have a valid node pointer");
		directChildNodes.push_back(node);
	}

	// todo: this should probably be called inside ned::begin() ned::end()
	std::sort(directChildNodes.begin(), directChildNodes.end(),
			  [](Node* const& l, Node* const& r) { return ned::GetNodePosition(l->ID).x < ned::GetNodePosition(r->ID).x; });

	for (Node* node : directChildNodes)
	{
		UpdateNodeOrdinals_Exec(data, node, curOrdinal);
	}
}

bool NodeEditor::Load(NodeEditorData& data)
{	
	if (!m_TargetTree)
		return false;

	if (!FileSystem::FileExists(data.treeGraphDataPath))
		return false;
	
	std::ifstream file(data.treeGraphDataPath);
	Json save = Json::parse(file);

	uintptr_t maxId = 1;

	if (save["TreeData"].empty())
	{
		return false;
	}

	Json& nodes = save["TreeData"]["Nodes"];
	data.nodes.reserve(nodes.size());
	for (const Json& jn : nodes)
	{
		uintptr_t id = jn["ID"];
		if (id > maxId) maxId = id;
		NodeKind nodeKind = jn["Kind"].get<NodeKind>();
		Node* node = nullptr;
		std::string nodeName = jn["NodeData"].get<std::string>();

		switch (nodeKind)
		{
		case NodeKind::Root:
		{
			uintptr_t outPinId = jn["Outputs"][0]["ID"];
			if (outPinId > maxId) maxId = outPinId;			
			node = &SpawnRootNode(data, (BTCompositeNode*)m_TargetTree->FindNodeByName(nodeName),id, outPinId);
			break;
		}
		case NodeKind::Selector:
		{
			uintptr_t outPinId = jn["Outputs"][0]["ID"];
			if (outPinId > maxId) maxId = outPinId;
			uintptr_t inPinId = jn["Inputs"][0]["ID"];
			if (inPinId > maxId) maxId = inPinId;
			node = &SpawnNodeFromBTree(data, m_TargetTree->FindNodeByName(nodeName), id, inPinId, outPinId);
			break;
		}
		case NodeKind::Sequence:
		{
			uintptr_t outPinId = jn["Outputs"][0]["ID"];
			if (outPinId > maxId) maxId = outPinId;
			uintptr_t inPinId = jn["Inputs"][0]["ID"];
			if (inPinId > maxId) maxId = inPinId;
			node = &SpawnNodeFromBTree(data, m_TargetTree->FindNodeByName(nodeName), id, inPinId, outPinId);
			break;
		}
		case NodeKind::Task:
		{
			uintptr_t inPinId = jn["Inputs"][0]["ID"];
			if (inPinId > maxId) maxId = inPinId;
			node = &SpawnNodeFromBTree(data, m_TargetTree->FindNodeByName(nodeName), id, inPinId);
			break;
		}
		case NodeKind::Deco:
		{
			uintptr_t outPinId = jn["Outputs"][0]["ID"];
			if (outPinId > maxId) maxId = outPinId;
			uintptr_t inPinId = jn["Inputs"][0]["ID"];
			if (inPinId > maxId) maxId = inPinId;
			node = &SpawnNodeFromBTree(data, m_TargetTree->FindNodeByName(nodeName), id, inPinId, outPinId);
			break;
		}
		}
	}

	data.nextId = maxId + 1;

	Json& links = save["TreeData"]["Links"];
	data.links.reserve(links.size());
	for (const Json& jl : links)
	{
		uintptr_t StartPinID = jl["StartPinID"];
		uintptr_t EndPinID = jl["EndPinID"];
		Link& link = data.links.emplace_back(Link(GetNextId(data), StartPinID, EndPinID));
		// todo:
		//link.Color = GetIconColor(startPin->Type);

	}

	BuildNodes(data);

	data.editorData = save["EditorData"];

	data.isDirty = false;

	return true;
}

void NodeEditor::Save(NodeEditorData& data)
{
	Json save;
	//
	save["TreeData"] = Json();
	save["EditorData"] = Json();

	Json& nodes = save["TreeData"]["Nodes"];
	int i = 0;
	for (const Node& node : data.nodes)
	{
		Json& jn = nodes[i++];
		jn["ID"] = node.ID.Get();
		jn["Type"] = node.Type;
		jn["Kind"] = node.Kind;
		jn["Ordinal"] = node.Ordinal;
		jn["NodeData"] = node.NodeData->GetName();
		//jn["taskData"] = node.taskData;
		Json& inputs = jn["Inputs"];
		size_t j = 0;
		for (const Pin& input : node.Inputs)
		{
			inputs[j++]["ID"] = input.ID.Get();
		}
		Json& outputs = jn["Outputs"];
		j = 0;
		for (const Pin& output : node.Outputs)
		{
			outputs[j++]["ID"] = output.ID.Get();
		}
		
	}

	Json& links = save["TreeData"]["Links"];
	i = 0;
	for (const Link& link : data.links)
	{
		Json& jl = links[i++];
		jl["StartPinID"] = link.StartPinID.Get();
		jl["EndPinID"] = link.EndPinID.Get();
	}

	//// todo: subtract the min id from all ids to combat limited number of long long numbers? -lot's of saving/loading might at some point break the saves

	save["EditorData"] = data.editorData;

	std::ofstream file(data.treeGraphDataPath);
	file << save.dump(4) << std::endl;

	//data.isDirty = false;
}

BTNode* NodeEditor::GetTargetNode(const ned::NodeId& NodeID)
{
	if (m_TargetTree)
	{
		size_t nodeSize = m_NodeEditorDataPerTree[m_TargetTree].nodes.size();
		for(size_t i=0;i<nodeSize;i++)
		{
			if(m_NodeEditorDataPerTree[m_TargetTree].nodes[i].ID == NodeID)
			{
				return m_NodeEditorDataPerTree[m_TargetTree].nodes[i].NodeData;
			}
		}
	}

	return nullptr;
}
