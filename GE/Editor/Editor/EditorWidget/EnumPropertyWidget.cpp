#include "PCH.h"
#include "EnumPropertyWidget.h"
#include "imgui.h"

EnumPropertyWidget::EnumPropertyWidget()
{

}

EnumPropertyWidget::~EnumPropertyWidget()
{

}

void EnumPropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	TPropertyHandle<XEnum>* Handle = GetHandle();

	if (!Handle)
		return;

	XEnum* val = Handle->GetValuePtr();


	m_EnumValue = val->GetUnderlying();


	if (ImGui::Combo(name.c_str(), &m_EnumValue, val->Names(), (int32)val->Count()))
	{
		val->Set(m_EnumValue);
	}
}

int32 EnumPropertyWidget::GetArraySize() const
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void EnumPropertyWidget::ArrayElementClear()
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
}

void EnumPropertyWidget::AddArrayElement()
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{		
		val->push_back(XEnum());
	}
}

void EnumPropertyWidget::ArrayElementGUI(int32 idx)
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		std::stringstream ss;
		ss << "##" << GetName() << "_" << idx;

		XEnum* val_idx = &(*val)[idx];


		m_EnumValues[idx] = val_idx->GetUnderlying();


		if (ImGui::Combo(ss.str().c_str(), &m_EnumValues[idx], val_idx->Names(), (int32)val_idx->Count()))
		{
			val_idx->Set(m_EnumValues[idx]);
		}
		
	}
}

void EnumPropertyWidget::InsertArrayElement(int32 idx)
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		XEnum t;
		t.Set(0);
		val->insert((val->begin() + idx), t);
	}
}

void EnumPropertyWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx));
}

void EnumPropertyWidget::DeleteArrayElement(int32 idx)
{
	std::vector<XEnum>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}

