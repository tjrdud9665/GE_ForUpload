#include "PCH.h"
#include "PropertyWidget.h"
#include "imgui.h"
#include <Path/PathManager.h>
#include <Renderer/Renderer.h>
#include "imgui_internal.h"

PropertyWidget::PropertyWidget()
	:m_IsArray(false)
{

}

PropertyWidget::~PropertyWidget()
{
	SAFE_DELETE(m_Prop);
}



void PropertyWidget::Initialize(PropertyHandle* InProperty, bool IsArray)
{
	m_Prop = InProperty;
	m_IsArray = IsArray;


	//view Only
	if (m_Prop->HasTag(DetailsView))
		SetDisableFlag(true);
}

void PropertyWidget::Update(float DeltaSeconds)
{
	const std::string name = "##" + GetName();

	if(m_IsArray)
	{
		int32 arraySize = GetArraySize();
		if (arraySize == INDEX_NONE)
		{
			ImGui::Text("Override PropertyWidget::GetArraySize()!");
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);		
			
			ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

			if (!arraySize)
				flag |= ImGuiTreeNodeFlags_Leaf;

			
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);								
			bool nodeOpen = ImGui::TreeNodeEx(GetName().c_str(), flag);
			ImGui::PopItemFlag();			

			ImGui::AlignTextToFramePadding();
			ImGui::TableSetColumnIndex(1);

			ImGui::Text("%d Array Elements", arraySize);
			ImGui::SameLine();
			if (ImGui::ImageButton(m_PlusIcon,ImVec2(12, 12)))
			{				
				//Add
				AddArrayElement();
			}
			ImGui::SameLine();
			if (ImGui::ImageButton(m_DeleteIcon, ImVec2(12, 12)))
			{
				//Clear 
				ArrayElementClear();
				arraySize = 0;
			}

			if (nodeOpen)
			{
				for (int32 i = 0; i < arraySize; i++)
				{
					//Element'sName
					std::string elemName = name + std::to_string(i).c_str();
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);					
					ImGui::Separator();
					ImGui::AlignTextToFramePadding();
					ImGui::Text(std::to_string(i).c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::Separator();
					//////////////////////////////////////////////////////////////////////////				
					ArrayElementGUI(i);				
					//////////////////////////////////////////////////////////////////////////
					ImGui::SameLine();
					
					std::string btnName = elemName + "action";
					std::string popUpname = btnName + "popup";

					if (ImGui::ArrowButton(btnName.c_str(), ImGuiDir_Down))
					{
						ImGui::OpenPopup(popUpname.c_str());
					}
					if (ImGui::BeginPopup(popUpname.c_str()))
					{
						if (ImGui::Selectable("Insert"))
						{					
							InsertArrayElement(i);//Add Default IN Pos
						}
						if (ImGui::Selectable("Delete"))
						{
							DeleteArrayElement(i);
							arraySize = 0; // no more loop ->just work Next Frame
							//arraySize = GetArraySize();							
						}
						if (ImGui::Selectable("Duplicate"))
						{														
							DuplicateArrayElement(i);//Just Next idx insert
						}

						ImGui::EndPopup();
					}
					AdditionalArrayElementGUI(i, DeltaSeconds);

				}
				ImGui::TreePop();
			}
			ImGui::EndTable();
		}

		ImGui::PopStyleVar();
	}
	else
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			BeginProperty();
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text(GetName().c_str());
			ImGui::TableSetColumnIndex(1);
			//////////////////////////////////////////
			PropertyGUI();
			//////////////////////////////////////////
			ImGui::EndTable();

			EndProperty();

			if (IsPropertyValueChanged())
			{
				PostEditProperties();
			}

		}

		ImGui::PopStyleVar();
	}


}

void PropertyWidget::PostEditProperties()
{

}

bool PropertyWidget::IsPropertyValueChanged() const
{	
	return ::memcmp(m_preEditedValue, m_postEditedValue, GetValueSize()) != 0;
}

size_t PropertyWidget::GetValueSize() const
{
	return 0;
}

void PropertyWidget::BeginProperty()
{
	//
	m_preEditedValue = nullptr;
}

void PropertyWidget::EndProperty()
{
	m_postEditedValue = nullptr;
}

int32 PropertyWidget::GetArraySize() const
{	
	return INDEX_NONE;
}

void PropertyWidget::ArrayElementClear()
{
	//Get ArrayHandle and Clear
	//val->clear();
}

void PropertyWidget::AddArrayElement()
{

}

void PropertyWidget::ArrayElementGUI(int32 idx)
{

}

void PropertyWidget::AdditionalArrayElementGUI(int32 idx, float DeltaSeconds)
{

}

void PropertyWidget::InsertArrayElement(int32 idx)
{

}

void PropertyWidget::DuplicateArrayElement(int32 idx)
{

}

void PropertyWidget::DeleteArrayElement(int32 idx)
{

}

void PropertyWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	const SPathInfo* editResPathInfo = g_Engine->Get<PathManager>()->FindPath(EDIT_RES_PATH);
	

	if (nullptr == m_DeleteIcon)
	{
		FTextureID deleteIconID = g_Engine->Get<Renderer>()->CreateTextureFromFile("delete.png", editResPathInfo->aPath);
		m_DeleteIcon = (ImTextureID)g_Engine->Get<Renderer>()->GetTextureObject(deleteIconID)->GetSRV();
	}

	if (nullptr == m_PlusIcon)
	{
		FTextureID plusIconID = g_Engine->Get<Renderer>()->CreateTextureFromFile("plus.png", editResPathInfo->aPath);
		m_PlusIcon = (ImTextureID)g_Engine->Get<Renderer>()->GetTextureObject(plusIconID)->GetSRV();
	}
}

void* PropertyWidget::m_DeleteIcon = nullptr;

void* PropertyWidget::m_PlusIcon = nullptr;

PropertyWidget* PropertyWidget::NewPropertyWidget(EditorWidget* owner, PropertyHandle*& prop)
{
	if (prop && owner)
	{
		//타입에 따른 PropertyWidget을 등록하고 가지고온다.
		//만약 std::vector<value_type>인경우->ArrayWidgetProperty에서 다시 분배한다
		//단 재분배시 해당하는 프로퍼티의 핸들은 vector 의 핸들임으
		bool IsArray = prop->IsArrayType_();

		XClass* customWidgetClass = prop->GetClassDefinedWidgetClass();
		if (customWidgetClass)
		{
			PropertyWidget* propWidget = owner->New<PropertyWidget>(prop->GetName().c_str(), customWidgetClass);
			propWidget->Initialize(prop, IsArray);
			return propWidget;
		}

		
	}


	//Failed to create PropertyWidget 
	// Delete Property Handle;
	SAFE_DELETE(prop);
	return nullptr;
}
