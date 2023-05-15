#include "PCH.h"
#include "StringPropertyWidget.h"
#include "imgui_stl.h"

StringPropertyWidget::StringPropertyWidget()
{

}

StringPropertyWidget::~StringPropertyWidget()
{

}

void StringPropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	std::string* const val = GetHandle()->GetValuePtr();

	if (val)
	{		
		ImGui::InputText(name.c_str(), *val);		
	}

}

int32 StringPropertyWidget::GetArraySize() const
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void StringPropertyWidget::ArrayElementClear()
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
	
}

void StringPropertyWidget::AddArrayElement()
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->push_back("");
}

void StringPropertyWidget::ArrayElementGUI(int32 idx)
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();
	std::stringstream ss;
	ss << "##" << GetName() << "_" << idx;	
	ImGui::InputText(ss.str().c_str(), (*val)[idx]);
}

void StringPropertyWidget::InsertArrayElement(int32 idx)
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), "");
}

void StringPropertyWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx));
}

void StringPropertyWidget::DeleteArrayElement(int32 idx)
{
	std::vector<std::string>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}
