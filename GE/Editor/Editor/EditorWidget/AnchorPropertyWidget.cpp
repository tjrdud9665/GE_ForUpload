#include "PCH.h"
#include "AnchorPropertyWidget.h"
#include "imgui.h"



AnchorPropertyWidget::AnchorPropertyWidget()
{

}

AnchorPropertyWidget::~AnchorPropertyWidget()
{

}

void AnchorPropertyWidget::Update(float DeltaSeconds)
{
	std::string name = "##" + GetName();
	SAnchorData* const val = GetHandle()->GetValuePtr();



	//ImGui::DragFloat2((name + "_anchar_max").c_str(), (val->Anchors.Maximum.Data()), 1.0f, -1.0f, 1.0f, "%g");

	//ImGui::DragFloat2((name + "_anchar_max").c_str(), (val->Offsets.Left), 1.0f, -1.0f, 1.0f, "%g");


	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
	{

		//Min,Max에 따라 크기 혹은 offset

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("AnchorMinimum");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat2((name + "_anchar_max").c_str(), (val->Anchors.Minimum.Data()), 0.1f, -1.0f, 1.0f, "%g");

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("AnchorMaximum");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat2((name + "_anchar_max").c_str(), (val->Anchors.Maximum.Data()), 0.1f, -1.0f, 1.0f, "%g");

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Left(Pos.X)");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat((name + "_left").c_str(), &val->Offsets.Left);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Top(Pos.Y)");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat((name + "_top").c_str(), &val->Offsets.Top);



		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Right(Width)");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat((name + "_right").c_str(), &val->Offsets.Right);		


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Bottom(Height)");
		ImGui::TableSetColumnIndex(1);
		ImGui::DragFloat((name + "bottom").c_str(), &val->Offsets.Bottom);
		

		ImGui::EndTable();

	}

	ImGui::PopStyleVar();
}

