#include "PCH.h"
#include "Vector2PropertyWidget.h"
#include "imgui.h"

Vector2PropertyWidget::Vector2PropertyWidget()
{

}

Vector2PropertyWidget::~Vector2PropertyWidget()
{

}

void Vector2PropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	SVector2* const val = GetHandle()->GetValuePtr();
	//ImGui::InputFloat3(name.c_str(), &(val->x), "%g");
	ImGui::DragFloat2(name.c_str(), &(val->x), 1.0f, -FLT_MAX, FLT_MAX, "%g");
}
