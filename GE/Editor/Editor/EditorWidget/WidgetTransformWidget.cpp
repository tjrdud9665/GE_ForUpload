#include "PCH.h"
#include "WidgetTransformWidget.h"
#include "imgui.h"


WidgetTransformWidget::WidgetTransformWidget()
{

}

WidgetTransformWidget::~WidgetTransformWidget()
{

}

void WidgetTransformWidget::Update(float DeltaSecondS)
{
	std::string name = "##" + GetName();
	SWidgetTransform* val = GetHandle()->GetValuePtr();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Translation");			
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat2((name + "_tr").c_str(), (val->Translation.Data()), 0.1f, -FLT_MAX, FLT_MAX, "%g");

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Scale");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat2((name + "_sc").c_str(), (val->Scale.Data()), 0.1f, -FLT_MAX, FLT_MAX, "%g");
		

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Angle");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat((name + "_rt").c_str(), &(val->Angle), 1.0f, -180.0f, 180.0f, "%g");

	


		ImGui::EndTable();

	}

	ImGui::PopStyleVar();

	
	
	
}

