#include "PCH.h"
#include "FloatPropertyWidget.h"
#include "imgui.h"

FloatPropertyWidget::FloatPropertyWidget()
{

}

FloatPropertyWidget::~FloatPropertyWidget()
{

}

void FloatPropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	ImGui::DragFloat(name.c_str(), GetHandle()->GetValuePtr());
}

int32 FloatPropertyWidget::GetArraySize() const
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void FloatPropertyWidget::ArrayElementClear()
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
}

void FloatPropertyWidget::AddArrayElement()
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->push_back(0.0f);
}

void FloatPropertyWidget::ArrayElementGUI(int32 idx)
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		std::stringstream ss;
		ss << "##" << GetName() << "_" << idx;
		ImGui::DragFloat(ss.str().c_str(), &(*val)[idx]);
	}

	
}

void FloatPropertyWidget::InsertArrayElement(int32 idx)
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), 0.0f);
}

void FloatPropertyWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx));
}

void FloatPropertyWidget::DeleteArrayElement(int32 idx)
{
	std::vector<float>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}
