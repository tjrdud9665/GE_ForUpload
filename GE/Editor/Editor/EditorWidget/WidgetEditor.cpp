#include "Assets/Widget/ImageWidget.h"
#include "PCH.h"
#include "WidgetEditor.h"
#include <Assets/Widget/UserWidget.h>
#include <Assets/Widget/PanelWidget.h>
#include <Assets/Widget/WidgetTree.h>
#include <Assets/Widget/ImageWidget.h>
#include <Assets/Widget/TextBlock.h>
#include "Assets/Widget/ProgressBar.h"
#include <Renderer/Renderer.h>
#include <RenderManager.h>
#include "PropertySet.h"
#include <Assets/Widget/ContentWidget.h>
#include <UI/UIManager.h>




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

static void DrawScale(const ImVec2& from, const ImVec2& to, float majorUnit, float minorUnit, float labelAlignment, float sign = 1.0f)
{
	auto drawList = ImGui::GetWindowDrawList();
	auto direction = (to - from) * ImInvLength(to - from, 0.0f);
	auto normal = ImVec2(-direction.y, direction.x);
	auto distance = sqrtf(ImLengthSqr(to - from));

	if (ImDot(direction, direction) < FLT_EPSILON)
		return;

	auto minorSize = 5.0f;
	auto majorSize = 10.0f;
	auto labelDistance = 8.0f;

	drawList->AddLine(from, to, IM_COL32(255, 255, 255, 255));

	auto p = from;
	for (auto d = 0.0f; d <= distance; d += minorUnit, p += direction * minorUnit)
		drawList->AddLine(p - normal * minorSize, p + normal * minorSize, IM_COL32(255, 255, 255, 255));

	for (auto d = 0.0f; d <= distance + majorUnit; d += majorUnit)
	{
		p = from + direction * d;

		drawList->AddLine(p - normal * majorSize, p + normal * majorSize, IM_COL32(255, 255, 255, 255));

		if (d == 0.0f)
			continue;

		char label[16];
		snprintf(label, 15, "%g", d * sign);
		auto labelSize = ImGui::CalcTextSize(label);

		auto labelPosition = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
		auto labelAlignedSize = ImDot(labelSize, direction);
		labelPosition += direction * (-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);
		labelPosition = ImFloor(labelPosition + ImVec2(0.5f, 0.5f));

		drawList->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label);
	}
}

WidgetEditor::WidgetEditor()
{

}

WidgetEditor::~WidgetEditor()
{

}

void WidgetEditor::Build()
{
	Super::Build();
	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoDocking);
	AddFlag(ImGuiWindowFlags_NoCollapse);

	m_widgetDetails = EditorWidget::AddNew<PropertySet>("WidgetDetails");
	m_slotDetails = EditorWidget::AddNew<PropertySet>("SlotDetials");
		
	m_panelWidgetClass = PanelWidget::StaticClass()->GetOwnChildren();

	m_conentWidgetClass = ContentWidget::StaticClass()->GetOwnChildren();

	std::vector<XClass*>::iterator iter = m_panelWidgetClass.begin();
	std::vector<XClass*>::iterator iter_end = m_panelWidgetClass.end();
	for (; iter != iter_end; )
	{
		if ((*iter)->HasTag(Abstract))
		{
			(iter) = m_panelWidgetClass.erase((iter));
			iter_end = m_panelWidgetClass.end();
			continue;
		}
		iter++;
	}

	m_imageWidgetClass = ImageWidget::StaticClass()->GetOwnChildren();
	m_imageWidgetClass.push_back(ImageWidget::StaticClass());	
	m_imageWidgetClass.push_back(ProgressBar::StaticClass());

	m_textWidgetClass = TextBlock::StaticClass()->GetOwnChildren();
	m_textWidgetClass.push_back(TextBlock::StaticClass());

}

void WidgetEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		ShowMenu();
		ImVec2 winSize = ImGui::GetWindowSize();
		winSize.x = std::max(winSize.x, 250.0f);
		winSize.y = std::max(winSize.y, 250.0f);
		ImGui::SetWindowSize(GetName().c_str(), winSize);
		if (m_TargetWidet)
		{
			m_TargetWidet->PreUpdate(DeltaSeconds);
			m_TargetWidet->Update(DeltaSeconds);
			m_TargetWidet->PostUpdate(DeltaSeconds);
		}
		//Canvas Draw
		DrawCanvas(DeltaSeconds);

		



		EndWindow();
	}
}

void WidgetEditor::DrawCanvas(float DeltaSeconds)
{
	auto availableRegion = ImGui::GetContentRegionAvail();

	static float s_SplitterSize = 6.0f;
	static float s_SplitterArea = 0.0f;
	static float s_LeftPaneSize = 0.0f;
	static float s_RightPaneSize = 0.0f;

	if (s_SplitterArea != availableRegion.x)
	{
		if (s_SplitterArea == 0.0f)
		{
			s_SplitterArea = availableRegion.x;
			s_LeftPaneSize = ImFloor(availableRegion.x * 0.25f);
			s_RightPaneSize = availableRegion.x - s_LeftPaneSize - s_SplitterSize;
		} else
		{
			auto ratio = availableRegion.x / s_SplitterArea;
			s_SplitterArea = availableRegion.x;
			s_LeftPaneSize = s_LeftPaneSize * ratio;
			s_RightPaneSize = availableRegion.x - s_LeftPaneSize - s_SplitterSize;
		}
	}

	
	static ImVec2 drawStartPoint;
	static bool isDragging = false;
	static ImRect panelRect;

	Splitter(true, s_SplitterSize, &s_LeftPaneSize, &s_RightPaneSize, 100.0f, 100.0f);
	

	auto canvasRect = m_canvas.Rect();
	auto viewRect = m_canvas.ViewRect();
	auto viewOrigin = m_canvas.ViewOrigin();
	auto viewScale = m_canvas.ViewScale();
	
	if(ImGui::BeginChild("##top", ImVec2(s_LeftPaneSize, -1), false, ImGuiWindowFlags_NoScrollWithMouse))
	{	

		ImGui::TextUnformatted("Origin:");
		ImGui::Indent();
		auto originChanged = false;
		ImGui::PushItemWidth(-ImGui::GetStyle().IndentSpacing);
		originChanged |= ImGui::DragFloat("##originx", &viewOrigin.x, 1.0f);
		originChanged |= ImGui::DragFloat("##originy", &viewOrigin.y, 1.0f);
		if (originChanged)
			m_canvas.SetView(viewOrigin, viewScale);
		ImGui::PopItemWidth();
		ImGui::Unindent();

		ImGui::TextUnformatted("Scale:");
		ImGui::Indent();
		ImGui::PushItemWidth(-ImGui::GetStyle().IndentSpacing);
		if (ImGui::DragFloat("##scale", &viewScale, 0.01f, 0.01f, 15.0f))
			m_canvas.SetView(viewOrigin, viewScale);
		ImGui::PopItemWidth();
		ImGui::Unindent();

		ImGui::Separator();		

		if (ImGui::BeginListBox("##WidgetPaletee",ImVec2(-FLT_MIN,0)))
		{
		
			if (ImGui::CollapsingHeader("Panel",ImGuiTreeNodeFlags_DefaultOpen))
			{
				size_t  panelClassSize = m_panelWidgetClass.size();
				for (size_t i = 0; i < panelClassSize; i++)
				{
					if (ImGui::Selectable(m_panelWidgetClass[i]->GetName().c_str()))
					{
						m_PaletteDrag = true;
					}

					if ( ImGui::BeginDragDropSource())
					{
						// 드래그 앤 드롭 시 전달할 데이터를 설정					
						
						ImGui::SetDragDropPayload("panelWidgetClass_idx", &i, sizeof(int32));

						// 드래그 앤 드롭을 수행하는 동안 표시할 텍스트를 설정
						ImGui::Text("Dragging...");
						ImGui::EndDragDropSource();
					}

				}
			}

			if (ImGui::CollapsingHeader("Content", ImGuiTreeNodeFlags_DefaultOpen))
			{
				size_t  contentClassSize = m_conentWidgetClass.size();
				for (size_t i = 0; i < contentClassSize; i++)
				{
					if (ImGui::Selectable(m_conentWidgetClass[i]->GetName().c_str()))
					{
						m_PaletteDrag = true;
					}

					if (ImGui::BeginDragDropSource())
					{
						// 드래그 앤 드롭 시 전달할 데이터를 설정					

						ImGui::SetDragDropPayload("contentWidgetClass_idx", &i, sizeof(int32));

						// 드래그 앤 드롭을 수행하는 동안 표시할 텍스트를 설정
						ImGui::Text("Dragging...");
						ImGui::EndDragDropSource();
					}

				}
			}

			if (ImGui::CollapsingHeader("Single", ImGuiTreeNodeFlags_DefaultOpen))
			{		
			
				size_t  imageClassSize = m_imageWidgetClass.size();
				for (size_t i = 0; i < imageClassSize; i++)
				{
					if (ImGui::Selectable(m_imageWidgetClass[i]->GetName().c_str()))
					{
						m_PaletteDrag = true;
					}

					if (ImGui::BeginDragDropSource())
					{
						// 드래그 앤 드롭 시 전달할 데이터를 설정					

						ImGui::SetDragDropPayload("imageWidgetClass_idx", &i, sizeof(int32));

						// 드래그 앤 드롭을 수행하는 동안 표시할 텍스트를 설정
						ImGui::Text("Dragging...");
						ImGui::EndDragDropSource();
					}

				}
				
			}
			if (ImGui::CollapsingHeader("Text", ImGuiTreeNodeFlags_DefaultOpen))
			{
				size_t  textClassSize = m_textWidgetClass.size();
				for (size_t i = 0; i < textClassSize; i++)
				{
					if (ImGui::Selectable(m_textWidgetClass[i]->GetName().c_str()))
					{
						m_PaletteDrag = true;
					}

					if (ImGui::BeginDragDropSource())
					{
						// 드래그 앤 드롭 시 전달할 데이터를 설정					

						ImGui::SetDragDropPayload("textWidgetClass_idx", &i, sizeof(int32));

						// 드래그 앤 드롭을 수행하는 동안 표시할 텍스트를 설정
						ImGui::Text("Dragging...");
						ImGui::EndDragDropSource();
					}

				}
			}			
			if (ImGui::CollapsingHeader("UserWidget", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::vector<XObject*> customCDO;
				std::vector<XClass*> userChild;
				size_t customCDOSize = UserWidget::StaticClass()->GetAllCustomCDO(OUT customCDO);

				UserWidget::StaticClass()->GetAllChildren(OUT userChild);
				size_t childsSize = userChild.size();
				for (size_t i = 0; i < childsSize; i++)
				{
					userChild[i]->GetAllCustomCDO(OUT customCDO);
				}
								
				customCDOSize = customCDO.size();

				m_userWidgetClass.resize(customCDOSize);
				for (size_t i = 0; i < customCDOSize; i++)
				{
					std::string relPathWithName;
					customCDO[i]->GetRelativePathWithName(OUT relPathWithName);
					m_userWidgetClass[i].CustomCDOName = relPathWithName;
				}
			
				
					
				

				size_t  userWidgetClassSize = m_userWidgetClass.size();
				for (size_t i = 0; i < userWidgetClassSize; i++)
				{
					if (ImGui::Selectable(m_userWidgetClass[i].CustomCDOName.c_str()))
					{
						m_PaletteDrag = true;
					}

					if (ImGui::BeginDragDropSource())
					{
						// 드래그 앤 드롭 시 전달할 데이터를 설정					

						ImGui::SetDragDropPayload("userWidgetClass_idx", &i, sizeof(int32));

						// 드래그 앤 드롭을 수행하는 동안 표시할 텍스트를 설정
						ImGui::Text("Dragging...");
						ImGui::EndDragDropSource();
					}

				}

			}
			ImGui::EndListBox();
		}

		//TreeNode 로 UI Tree 를 표현	
		if (m_TargetWidet)
		{		

			if (ImGui::BeginListBox("##WidgetTree", ImVec2(-FLT_MIN, 0)))
			{				
				if(ImGui::TreeNodeEx(m_TargetWidet->GetName().c_str()))
				{			
					if (ImGui::BeginDragDropTarget() && 
						!m_TargetWidet->GetWidgetTree()->GetRootWidget())
					{
						
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("panelWidgetClass_idx"))
						{
							
							int32* received_data = (int32*)(payload->Data);
							m_TargetWidet->GetWidgetTree()->SetRootWidget(m_TargetWidet->GetWidgetTree()->ConstructWidget<Widget>(m_panelWidgetClass[*received_data]));
							
						}
						//panelWidgetClass_idx
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("contentWidgetClass_idx"))
						{
							int32* received_data = (int32*)(payload->Data);
							m_TargetWidet->GetWidgetTree()->SetRootWidget(m_TargetWidet->GetWidgetTree()->ConstructWidget<Widget>(m_conentWidgetClass[*received_data]));
						}

						ImGui::EndDragDropTarget();
					}
					m_Hovered = nullptr;
					ShowWidgetTree(m_TargetWidet->GetWidgetTree(), m_TargetWidet->GetWidgetTree()->GetRootWidget());
			
					ImGui::TreePop();
				}
				
				HandleClicked();

				ImGui::EndListBox();
			}

			ImGui::Separator();

			ImGui::TextUnformatted("WidgetDetails");
			ImGui::Indent();
			ImGui::PushItemWidth(-ImGui::GetStyle().IndentSpacing);
			m_widgetDetails->OnUpdate(DeltaSeconds);
			ImGui::PopItemWidth();
			ImGui::Unindent();

			ImGui::Separator();

			ImGui::TextUnformatted("Slot");
			ImGui::Indent();
			ImGui::PushItemWidth(-ImGui::GetStyle().IndentSpacing);
			m_slotDetails->OnUpdate(DeltaSeconds);
			ImGui::PopItemWidth();
			ImGui::Unindent();
		}	
	
		ShowContextPop();
		ImGui::EndChild();
	}

	if (!ImGui::IsMouseDown(0))
	{
		m_PaletteDrag = false;
	}
	


	ImGui::SameLine(0.0f, s_SplitterSize);

	bool canvasHovered = false;
	if (m_canvas.Begin("##mycanvas", ImVec2(s_RightPaneSize, 0.0f)))
	{
		//auto drawList = ImGui::GetWindowDrawList();

		if ((isDragging || ImGui::IsItemHovered()) && ImGui::IsMouseDragging(1, 0.0f))
		{
			if (!isDragging)
			{
				isDragging = true;
				drawStartPoint = viewOrigin;
			}

			m_canvas.SetView(drawStartPoint + ImGui::GetMouseDragDelta(1, 0.0f) * viewScale, viewScale);
		} 
		else if (isDragging)
			isDragging = false;

		viewRect = m_canvas.ViewRect();

		if (viewRect.Max.x > 0.0f)
			DrawScale(ImVec2(0.0f, 0.0f), ImVec2(viewRect.Max.x, 0.0f), 100.0f, 10.0f, 0.6f);
		if (viewRect.Min.x < 0.0f)
			DrawScale(ImVec2(0.0f, 0.0f), ImVec2(viewRect.Min.x, 0.0f), 100.0f, 10.0f, 0.6f, -1.0f);
		if (viewRect.Max.y > 0.0f)
			DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, viewRect.Max.y), 100.0f, 10.0f, 0.6f);
		if (viewRect.Min.y < 0.0f)
			DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, viewRect.Min.y), 100.0f, 10.0f, 0.6f, -1.0f);

		/*ImGui::Text("Hovered:     %d", ImGui::IsItemHovered() ? 1 : 0);*/
		if (ImGui::IsItemHovered())
			canvasHovered = true;
			
		//LT
		//RB
		//Test..
		//Draw Pannel
		if (m_TargetWidet)
		{
			WidgetTree* tree = m_TargetWidet->GetWidgetTree();
			if(tree)
				DrawWidgetsInCanvas(tree);
		}
		
		

		//ImGui::Button("Panel", ImVec2(s_RightPaneSize * 0.75f, availableRegion.y * 0.5f) * 0.5f);
		//panelRect.Min = ImGui::GetItemRectMin();
		//panelRect.Max = ImGui::GetItemRectMax();
		
		m_canvas.End();
	}	
	
	if (canvasHovered && ImGui::GetIO().MouseWheel != 0.0f)
	{
		m_canvas.SetView(viewOrigin, viewScale + ImGui::GetIO().MouseWheel * 0.05f);
	}
	
	
	
}

void WidgetEditor::DrawWidgetsInCanvas(class WidgetTree* InWidgetTree)
{
	std::vector<Widget*> allWidgets;
	size_t allWidgetCnt = InWidgetTree->GetAllWidgets(OUT allWidgets);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	Renderer* rdr = g_Engine->Get<Renderer>();
	ImGui::Image(rdr->GetTextureSRV(rdr->GetRenderTargetTexture(g_Engine->Get<UIManager>()->m_PanelTarget)), ImVec2(1920, 1080));

	for(size_t i=0;i<allWidgetCnt;i++)
	{		
		ImU32 color = IM_COL32(255, 255, 255, 50);
		if (allWidgets[i] == m_Clicked)
		{
			color = IM_COL32(0, 255, 0, 255);
		}
	
		if(allWidgets[i]->Slot)
		{
			std::vector<SVertex_PUV> vtx = allWidgets[i]->Slot->GetGeometryVertices();

	
			//0 ,1 ,2 ,5
			SVector2 Lt = (vtx[1].Position + SVector2(1.0f, 1.0f)) * 0.5f;
			SVector2 Rt = (vtx[2].Position + SVector2(1.0f, 1.0f)) * 0.5f;
			SVector2 Rb = (vtx[5].Position + SVector2(1.0f, 1.0f)) * 0.5f;
			SVector2 Lb = (vtx[0].Position + SVector2(1.0f, 1.0f)) * 0.5f;
			//0~~1

			SVector2 scrSize = g_Engine->Get<Renderer>()->FrameRenderTargetDimensionsAsFloat2();
				//allWidgets[i]->Slot->GetParent()->GetDesiredSize();
			scrSize.y *= -1.0f;
			
			ImVec2 p0 = ImVec2(Lb.x * scrSize.x, Lb.y * scrSize.y - scrSize.y);
			ImVec2 p1 = ImVec2(Lt.x * scrSize.x, Lt.y * scrSize.y - scrSize.y);
			ImVec2 p2 = ImVec2(Rt.x * scrSize.x, Rt.y * scrSize.y - scrSize.y);
			ImVec2 p3 = ImVec2(Rb.x * scrSize.x, Rb.y * scrSize.y - scrSize.y);

			draw_list->AddLine(p0, p1, color);
			draw_list->AddLine(p1, p2, color);
			draw_list->AddLine(p2, p3, color);
			draw_list->AddLine(p3, p0, color);

		}
	
	}
}

void WidgetEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_TargetWidet)
				{					
					m_TargetWidet->Save();		
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

bool WidgetEditor::ShowWidgetTree(class WidgetTree* widgetTree, class Widget* InParet)
{
	if (!InParet)
		return false;

	std::vector<Widget*> childs;
	WidgetTree::GetOwnChildWidgets(InParet, OUT childs);
	size_t childSize = childs.size();


	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_SpanFullWidth  | ImGuiTreeNodeFlags_DefaultOpen;
	if (m_Clicked == InParet)
		flag |= ImGuiTreeNodeFlags_Selected;
	if(childSize ==0)
		flag |= ImGuiTreeNodeFlags_Leaf;

	bool AcceptDrop = false;
	bool bOpen = ImGui::TreeNodeEx((void*)InParet, flag, InParet->GetName().c_str());

	if(InParet->GetClass()->IsChildOf(PanelWidget::StaticClass()))
	{
		PanelWidget* castPanel = (PanelWidget*)InParet;
	
		if (ImGui::BeginDragDropTarget() && !AcceptDrop)
		{
			if (castPanel && castPanel->CanAddMoreChildren())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("panelWidgetClass_idx"))
				{
					int32* received_data = (int32*)(payload->Data);
					castPanel->AddChild(m_TargetWidet->GetWidgetTree()->ConstructWidget<Widget>(m_panelWidgetClass[*received_data],"",true));
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("imageWidgetClass_idx"))
				{
					int32* received_data = (int32*)(payload->Data);
					castPanel->AddChild(m_TargetWidet->GetWidgetTree()->ConstructWidget<Widget>(m_imageWidgetClass[*received_data],"",true));
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("contentWidgetClass_idx"))
				{
					int32* received_data = (int32*)(payload->Data);
					castPanel->AddChild(m_TargetWidet->GetWidgetTree()->ConstructWidget<Widget>(m_conentWidgetClass[*received_data], "", true));
				}

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("userWidgetClass_idx"))
				{
					int32* received_data = (int32*)(payload->Data);
					castPanel->AddChild(m_TargetWidet->GetWidgetTree()->ConstructWidget<UserWidget>(m_userWidgetClass[*received_data], m_userWidgetClass[*received_data].CustomCDOName, true));
				}			

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("textWidgetClass_idx"))
				{
					int32* received_data = (int32*)(payload->Data);
					castPanel->AddChild(m_TargetWidet->GetWidgetTree()->ConstructWidget<TextBlock>(m_textWidgetClass[*received_data], "", true));
				}
				//
			}
			AcceptDrop = true;
			ImGui::EndDragDropTarget();
		}
	
	}

	if (bOpen)
	{	
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
		{
			m_Hovered = InParet;
		}

		for (size_t i = 0; i < childSize; i++)
		{
			AcceptDrop |= ShowWidgetTree(widgetTree, childs[i]);
		}		

		ImGui::TreePop();
	}

	return AcceptDrop;


}

void WidgetEditor::SetTargetWidget(class UserWidget* InTargetWidget)
{
	m_TargetWidet =  InTargetWidget;

	m_Hovered = m_Clicked = nullptr;

	if (m_widgetDetails)
	{
		m_widgetDetails->SetPropertyTarget(nullptr);
	}
	if (m_slotDetails)
	{
		m_slotDetails->SetPropertyTarget(nullptr);
	}
}

void WidgetEditor::Render()
{
	if(m_TargetWidet)
	{
		UIManager* uiManager = g_Engine->Get<UIManager>();
	
		Renderer* rnder =g_Engine->Get<Renderer>();
		if(rnder)
			m_TargetWidet->Render(rnder);

		rnder->ForceClearSrv();
		uiManager->BeginUI();

		uiManager->RenderUI();

		uiManager->EndUI();
	}

	
}

void WidgetEditor::PreRender()
{

}

void WidgetEditor::HandleClicked()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	const bool left_click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	const bool right_click = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	if (is_window_hovered)
	{
		if (left_click && m_Hovered)
		{
			m_Clicked = m_Hovered;

			if (m_widgetDetails)
			{
				m_widgetDetails->SetPropertyTarget(m_Clicked);
			}
			if (m_slotDetails)
			{
				m_slotDetails->SetPropertyTarget(m_Clicked->GetPanelSlot());
			}
		}
		if (right_click && m_Hovered)
		{
			m_Clicked = m_Hovered;
			if (m_widgetDetails)
			{
				m_widgetDetails->SetPropertyTarget(m_Clicked);
			}
			if (m_slotDetails)
			{
				m_slotDetails->SetPropertyTarget(m_Clicked->GetPanelSlot());
			}
			
			m_ShowWidgetContextPop = true;
		}
		else if((right_click || left_click)  && !m_Hovered)
		{
			m_ShowWidgetContextPop = false;
		}
	}



}

void WidgetEditor::ShowContextPop()
{
	if(m_ShowWidgetContextPop)
	{
		ImGui::OpenPopup("ObjectContext");
		if(ImGui::BeginPopup("ObjectContext"))
		{
			if (ImGui::Selectable("Delete"))
			{
				m_ShowWidgetContextPop= false;
				
				if (m_widgetDetails)
				{
					m_widgetDetails->SetPropertyTarget(nullptr);
				}
				if (m_slotDetails)
				{
					m_slotDetails->SetPropertyTarget(nullptr);
				}
				//Kill
				m_TargetWidet->GetWidgetTree()->RemoveWidget(m_Clicked);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	
	
}

