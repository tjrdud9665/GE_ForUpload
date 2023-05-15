#include "PCH.h"
#include "ColorPropertyWidget.h"
#include "imgui.h"

ColorPropertyWidget::ColorPropertyWidget()
{

}

ColorPropertyWidget::~ColorPropertyWidget()
{

}

void ColorPropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	SLinearColor* const val = GetHandle()->GetValuePtr();
	//ImGui::InputFloat3(name.c_str(), &(val->x), "%g");
	ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions;


	ImGui::ColorEdit4(name.c_str(), &(val->r), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
}
