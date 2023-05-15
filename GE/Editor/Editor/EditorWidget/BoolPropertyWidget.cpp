#include "PCH.h"
#include "BoolPropertyWidget.h"
#include "imgui.h"

BoolPropertyWidget::BoolPropertyWidget()
{

}


BoolPropertyWidget::~BoolPropertyWidget()
{

}

void BoolPropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	ImGui::Checkbox(name.c_str(), GetHandle()->GetValuePtr());
}

void BoolPropertyWidget::PostEditProperties()
{

}

void BoolPropertyWidget::BeginProperty()
{
	 m_Prev = *GetHandle()->GetValuePtr();
	 m_preEditedValue = &m_Prev;
}

void BoolPropertyWidget::EndProperty()
{
	m_postEditedValue = GetHandle()->GetValuePtr();
}

int32 BoolPropertyWidget::GetArraySize() const
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();

	if(val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void BoolPropertyWidget::ArrayElementClear()
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
}

void BoolPropertyWidget::AddArrayElement()
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->push_back(false);
}

void BoolPropertyWidget::ArrayElementGUI(int32 idx)
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		std::stringstream ss;
		ss << "##" << GetName() << "_" << idx;
		bool idxVal = (*val)[idx];
		ImGui::Checkbox(ss.str().c_str(), &idxVal);
		(*val)[idx] = idxVal;
	}
	
}

void BoolPropertyWidget::InsertArrayElement(int32 idx)
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), false);
}

void BoolPropertyWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx));
}

void BoolPropertyWidget::DeleteArrayElement(int32 idx)
{
	std::vector<bool>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}
