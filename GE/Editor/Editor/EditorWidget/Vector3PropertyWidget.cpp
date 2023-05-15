#include "PCH.h"
#include "Vector3PropertyWidget.h"
#include "imgui_stl.h"

Vector3PropertyWidget::Vector3PropertyWidget()
{

}

Vector3PropertyWidget::~Vector3PropertyWidget()
{

}

void Vector3PropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	SVector3* const val = GetHandle()->GetValuePtr();
	//ImGui::InputFloat3(name.c_str(), &(val->x), "%g");
	ImGui::DragFloat3(name.c_str(), &(val->x), 0.1f, -FLT_MAX, FLT_MAX, "%g");
}
