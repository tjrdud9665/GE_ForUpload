#include "PCH.h"
#include "SubClassWidget.h"
#include "imgui.h"

 SubClassWidget::SubClassWidget()
{

}

 SubClassWidget::~SubClassWidget()
 {

 }

 void SubClassWidget::PropertyGUI()
 {
	 std::string name = "##" + GetName();
	 TPropertyHandle<XSubclassOf>* Handle = GetHandle();

	 if (!Handle)
		 return;

	 XSubclassOf* val = Handle->GetValuePtr();

	 if (!val->Class)
		 return;

	 std::vector<XClass*> childs;
	 val->TemplatedClass->GetAllChildren(OUT childs);
	 if (!val->TemplatedClass->HasTag(Abstract))
		 childs.push_back(val->TemplatedClass);

	 size_t childSize = childs.size();

	 std::string selectedClassName = val->CustomCDOName;
	 if (selectedClassName.empty())
		 selectedClassName = val->Class->GetName();

	 if (ImGui::BeginCombo(name.c_str(), selectedClassName.c_str()))
	 {
		 for (size_t i = 0; i < childSize; i++)
		 {
			 bool IsSelected = val->Class == childs[i];
			 if (ImGui::Selectable(childs[i]->GetName().c_str(), IsSelected))
			 {
				 (*val) = childs[i];
				 (*val).CustomCDOName = "";
			 }
		 }

		 ImGui::Text("--CDO--");
		 ImGui::Separator();
		 for (size_t i = 0; i < childSize; i++)
		 {
			 std::vector<XObject*> customCDO;
			 size_t customCDOSize = childs[i]->GetAllCustomCDO(OUT customCDO);
			 for (size_t j = 0; j < customCDOSize; j++)
			 {
				 std::string relPathWithName;
				 customCDO[j]->GetRelativePathWithName(OUT relPathWithName);

				 bool IsSelected = val->CustomCDOName == relPathWithName;
				 if (ImGui::Selectable(relPathWithName.c_str(), IsSelected))
				 {
					 (*val) = customCDO[j];
				 }
			 }
		 }

		 ImGui::EndCombo();
	 }
 }

 void SubClassWidget::Initialize(PropertyHandle* InProperty, bool IsArray)
 {
	 Super::Initialize(InProperty, IsArray);

	 if(IsArray)
	 {			 
		 std::vector<XClass*> allClass;
		 XObject::StaticClass()->GetAllChildren(OUT allClass);
		 size_t classSize = allClass.size();

		 for(size_t i =0;i<classSize;i++)
		 {		 
			 std::string tester = "vector<TSubclassOf<";
			 if(InProperty->GetTypeName().find(allClass[i]->GetName()) == tester.size())
			 {
				 m_ArrayDefaultClass = allClass[i];
				 break;
			 }			 
		 }		 
	 }

 }

 int32 SubClassWidget::GetArraySize() const
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();

	 if (val)
	 {
		 return (int32)val->size();
	 }

	 return INDEX_NONE;
 }

 void SubClassWidget::ArrayElementClear()
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();
	 if (val)
		 val->clear();
 }

 void SubClassWidget::AddArrayElement()
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();
	 if (val)
	 {		
		 val->push_back(XSubclassOf(m_ArrayDefaultClass));
	 }
 }

 void SubClassWidget::ArrayElementGUI(int32 idx)
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();
	 if (val)
	 {
		 std::stringstream ss;
		 ss << "##" << GetName() << "_" << idx;		 
		 std::string name = "##" + GetName();
		 TPropertyHandle<std::vector<XSubclassOf>>* Handle = GetArrayHandle();

		 if (!Handle)
			 return;

		 std::vector<XSubclassOf>* valarr = Handle->GetValuePtr();
		 if (!valarr)
			 return;

		 XSubclassOf* val = &(*valarr)[idx];

		 if (!val->Class)
			 return;

		 std::vector<XClass*> childs;
		 val->TemplatedClass->GetAllChildren(OUT childs);
		 if (!val->TemplatedClass->HasTag(Abstract))
			 childs.push_back(val->TemplatedClass);

		 size_t childSize = childs.size();

		 std::string selectedClassName = val->CustomCDOName;
		 if (selectedClassName.empty())
			 selectedClassName = val->Class->GetName();

		 if (ImGui::BeginCombo(ss.str().c_str(), selectedClassName.c_str()))
		 {
			 for (size_t i = 0; i < childSize; i++)
			 {
				 bool IsSelected = val->Class == childs[i];
				 if (ImGui::Selectable(childs[i]->GetName().c_str(), IsSelected))
				 {
					 (*val) = childs[i];
					 (*val).CustomCDOName = "";
				 }
			 }

			 ImGui::Text("--CDO--");
			 ImGui::Separator();
			 for (size_t i = 0; i < childSize; i++)
			 {
				 std::vector<XObject*> customCDO;
				 size_t customCDOSize = childs[i]->GetAllCustomCDO(OUT customCDO);
				 for (size_t j = 0; j < customCDOSize; j++)
				 {
					 std::string relPathWithName;
					 customCDO[j]->GetRelativePathWithName(OUT relPathWithName);

					 bool IsSelected = val->CustomCDOName == relPathWithName;
					 if (ImGui::Selectable(relPathWithName.c_str(), IsSelected))
					 {
						 (*val) = customCDO[j];
					 }
				 }
			 }

			 ImGui::EndCombo();
		 }
	 }

 }

 void SubClassWidget::InsertArrayElement(int32 idx)
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();
	 if (val)
		 val->insert((val->begin() + idx), XSubclassOf(m_ArrayDefaultClass));
 }

 void SubClassWidget::DuplicateArrayElement(int32 idx)
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();
	 if (val)
	 {
		 val->insert((val->begin() + idx), val->at(idx));
	 }
 }

 void SubClassWidget::DeleteArrayElement(int32 idx)
 {
	 std::vector<XSubclassOf>* val = GetArrayHandle()->GetValuePtr();
	 if (val)
		 val->erase((val->begin() + idx));
 }

