#include "PCH.h"
#include "SignedPropertyWidget.h"
#include "imgui.h"

SignedPropertyWidget::SignedPropertyWidget()
{

}

SignedPropertyWidget::~SignedPropertyWidget()
{

}

void SignedPropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	ImGui::InputInt(name.c_str(), GetHandle()->GetValuePtr());
}

int32 SignedPropertyWidget::GetArraySize() const
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void SignedPropertyWidget::ArrayElementClear()
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
}

void SignedPropertyWidget::AddArrayElement()
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->push_back(0);
}

void SignedPropertyWidget::ArrayElementGUI(int32 idx)
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		std::stringstream ss;
		ss << "##" << GetName() << "_" << idx;
		ImGui::InputInt(ss.str().c_str(), &(*val)[idx]);
	}
}

void SignedPropertyWidget::InsertArrayElement(int32 idx)
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), 0);
}

void SignedPropertyWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx));
}

void SignedPropertyWidget::DeleteArrayElement(int32 idx)
{
	std::vector<int32>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}
