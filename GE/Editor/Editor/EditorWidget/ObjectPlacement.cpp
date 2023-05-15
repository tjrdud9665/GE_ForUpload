#include "PCH.h"
#include "ObjectPlacement.h"
#include "ObjectListWidget.h"
#include "imgui.h"
#include "imgui_stl.h"
#include "GameFramework/GameObject/GameObject.h"
#include "ObjectManager.h"
#include "Physics/PhysicsSystem.h"


ObjectPlacement::ObjectPlacement()
	:m_SpawnScale(SVector3(1.0f))
	, m_SpawnLocation(SVector3(0.0f))
	, m_SpawnRotation(SVector3(0.0f))
{

}

ObjectPlacement::ObjectPlacement(const ObjectPlacement& Other)
	:Super(Other)
	, m_SpawnScale(SVector3(1.0f))
	, m_SpawnLocation(SVector3(0.0f))
	, m_SpawnRotation(SVector3(0.0f))
{
	
}

ObjectPlacement::~ObjectPlacement()
{

}

void ObjectPlacement::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	
}

void ObjectPlacement::Build()
{
	m_ObjectLists = EditorWidget::AddNew<ObjectListWidget>("ObjectList");
}

void ObjectPlacement::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		UpdateChilds(DeltaSeconds);
		ShowSpawnButton();
		EndWindow();
	}
}

void ObjectPlacement::ShowSpawnButton()
{	
	ObjectManager* objMgr = g_Engine->Get<ObjectManager>();
	if (objMgr)
	{
				
		if (ImGui::BeginTable("objectDefaultProp", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("ObjectName");
			ImGui::TableSetColumnIndex(1);
			ImGui::InputText("##ObjectName", m_SpawnObjectName);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("WorldLocation");
			ImGui::TableSetColumnIndex(1);
			ImGui::DragFloat3("##position", m_SpawnLocation.Data(), 1.0f, -FLT_MAX, FLT_MAX, "%g");

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("WorldRotation");
			ImGui::TableSetColumnIndex(1);
			ImGui::DragFloat3("##rotation", m_SpawnRotation.Data(), 1.0f, -FLT_MAX, FLT_MAX, "%g");

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("WorldScale");
			ImGui::TableSetColumnIndex(1);
			ImGui::DragFloat3("##scale", m_SpawnScale.Data(), 1.0f, -FLT_MAX, FLT_MAX, "%g");



			ImGui::EndTable();

		}

		if (ImGui::Button("SpawnGameObject"))
		{
			if(m_SpawnObjectName.empty())
			{
				m_SpawnObjectName = m_ObjectLists->GetClickedClass().Class->GetName();
				if(!m_ObjectLists->GetClickedClass().CustomCDOName.empty())
					m_SpawnObjectName = m_ObjectLists->GetClickedClass().CustomCDOName;

			}
			GameObject* spawnedObject =objMgr->SpawnGameObject<GameObject>(m_SpawnObjectName, m_ObjectLists->GetClickedClass());

			spawnedObject->SetObjectLocation(m_SpawnLocation);
			spawnedObject->SetObjectScale(m_SpawnScale);
			spawnedObject->SetObjectRotation(m_SpawnRotation);
			

		}
	}
}

