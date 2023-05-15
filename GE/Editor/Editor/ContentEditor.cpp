#include "PCH.h"
#include "ContentEditor.h"
#include "EditorWidget/AssetViewer.h"
#include "EditorWidget/MaterialEditor.h"
#include "Assets/Material/Material.h"
#include "EditorWidget/MeshEditor.h"
#include "Assets/Mesh/MeshAsset.h"
#include "GameFramework/GameObject/GameObject.h"
#include "EditorWidget/GameObjectEditor.h"
#include "Assets/Scene/Scene.h"
#include "SceneManager.h"
#include "EditorWidget/SceneInfoEditor.h"
#include "EditorWidget/AnimationEditor.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include "EditorWidget/GameplayAbilityEditor.h"
#include <Assets/GameplayAbility/GameplayAbility.h>
#include <Assets/GameplayEffect/GameplayEffect.h>
#include "EditorWidget/GameplayEffectEditor.h"
#include "EditorWidget/NodeEditor.h"
#include <Assets/AI/BlackBoard.h>
#include "EditorWidget/BlackboardEditor.h"
#include "EditorWidget/SoundEditor.h"
#include <Assets/Sound/Sound.h>
#include "EditorWidget/WidgetEditor.h"
#include <Assets/Widget/UserWidget.h>
#include <Assets/Particle/ParticleAsset.h>
#include "EditorWidget/ParticleEditor.h"


ContentEditor::ContentEditor()
{
	
}

ContentEditor::ContentEditor(const ContentEditor& Other)
	:Super(Other)
{
	assert(true);
}

ContentEditor::~ContentEditor()
{

}

void ContentEditor::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_AssetViewer = EditorWidget::CreateStandalone<AssetViewer>("AssetViewer", GetSelf());
	m_AssetViewer->SetOpen(true);
	
	m_MaterialEditor = EditorWidget::CreateStandalone<MaterialEditor>("MaterialEditor", GetSelf());
	m_MaterialEditor->SetOpen(false);

	m_MeshEditor = EditorWidget::CreateStandalone<MeshEditor>("MeshEditor", GetSelf());
	m_MeshEditor->SetOpen(false);

	m_ObjectEditor = EditorWidget::CreateStandalone<GameObjectEditor>("GameObjectEditor", GetSelf());
	m_ObjectEditor->SetOpen(false);

	m_SceneInfoEditor = EditorWidget::CreateStandalone<SceneInfoEditor>("SceneInfoEditor", GetSelf());
	m_SceneInfoEditor->SetOpen(false);

	m_AnimationEditor = EditorWidget::CreateStandalone<AnimationEditor>("AnimEditor", GetSelf());
	m_AnimationEditor->SetOpen(false);

	m_AbilityEditor = EditorWidget::CreateStandalone<GameplayAbilityEditor>("AbilityEditor", GetSelf());
	m_AbilityEditor->SetOpen(false);

	m_EffectEditor = EditorWidget::CreateStandalone<GameplayEffectEditor>("EffectEditor", GetSelf());
	m_EffectEditor->SetOpen(false);

	m_SoundEditor = EditorWidget::CreateStandalone<SoundEditor>("SoundEditor", GetSelf());
	m_SoundEditor->SetOpen(false);


	m_NodeEditor = EditorWidget::CreateStandalone<NodeEditor>("BehaviorTreeEdior", GetSelf());
	m_NodeEditor->SetOpen(false);

	m_BBEditor = EditorWidget::CreateStandalone<BlackboardEditor>("BlackboardEditor", GetSelf());
	m_BBEditor->SetOpen(false);

	m_WidgetEidtor = EditorWidget::CreateStandalone<WidgetEditor>("WidgetEditor", GetSelf());
	m_WidgetEidtor->SetOpen(false);

	m_ParticleEditor = EditorWidget::CreateStandalone<ParticleEditor>("ParticleEditor", GetSelf());
	m_ParticleEditor->SetOpen(false);
}

void ContentEditor::ExpandViewsMenu()
{
	if (ImGui::BeginMenu("Contents Editor"))
	{
		if (ImGui::MenuItem("AssetViewer", (const char*)0, m_AssetViewer->IsOpenedPtr())) {}
		if (ImGui::MenuItem("MaterialEditor", (const char*)0, m_MaterialEditor->IsOpenedPtr())) {}
		if (ImGui::MenuItem("MeshEditor", (const char*)0, m_MeshEditor->IsOpenedPtr())) {}
		if (ImGui::MenuItem("GameObject", (const char*)0, m_ObjectEditor->IsOpenedPtr())) {}
		if (ImGui::MenuItem("SceneInfo", (const char*)0, m_SceneInfoEditor->IsOpenedPtr())) {}
		if (ImGui::MenuItem("Animation", (const char*)0, m_AnimationEditor->IsOpenedPtr())) {}
		if (ImGui::MenuItem("Sound", (const char*)0, m_SoundEditor->IsOpenedPtr())) {}

		if (ImGui::MenuItem("Ability", (const char*)0, m_AbilityEditor->IsOpenedPtr())) {}
		if (ImGui::MenuItem("Effect", (const char*)0, m_EffectEditor->IsOpenedPtr())) {}

		

		ImGui::EndMenu();
	}	

}

void ContentEditor::Update(float DeltaSeconds)
{
	m_AssetViewer->OnUpdate(DeltaSeconds);
	m_MaterialEditor->OnUpdate(DeltaSeconds);
	m_MeshEditor->OnUpdate(DeltaSeconds);
	m_ObjectEditor->OnUpdate(DeltaSeconds);
	m_SceneInfoEditor->OnUpdate(DeltaSeconds);
	m_SoundEditor->OnUpdate(DeltaSeconds);

	m_AnimationEditor->OnUpdate(DeltaSeconds);
	m_AbilityEditor->OnUpdate(DeltaSeconds);
	m_EffectEditor->OnUpdate(DeltaSeconds);


	m_NodeEditor->OnUpdate(DeltaSeconds);
	m_BBEditor->OnUpdate(DeltaSeconds);

	m_WidgetEidtor->OnUpdate(DeltaSeconds);
	m_ParticleEditor->OnUpdate(DeltaSeconds);

}

void ContentEditor::OnClickedAsset(class AssetData* ClickedAsset)
{
	if (ClickedAsset->GetClass()->IsA(Material::StaticClass()))
	{
		m_MaterialEditor->SetOpen(true);
		m_MaterialEditor->SetEditMaterial((Material*)ClickedAsset);
	} 
	else if(ClickedAsset->GetClass()->IsChildOf(MeshAsset::StaticClass()) ||
			ClickedAsset->GetClass()->IsA(MeshAsset::StaticClass()))
	{
		m_MeshEditor->SetOpen(true);
		m_MeshEditor->SetTargetMesh((MeshAsset*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsChildOf(GameObject::StaticClass()) ||
			 ClickedAsset->GetClass()->IsA(GameObject::StaticClass()))
	{
		m_ObjectEditor->SetOpen(true);
		m_ObjectEditor->SetTargetObject((GameObject*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsChildOf(GameplayAbility::StaticClass()) ||
			 ClickedAsset->GetClass()->IsA(GameplayAbility::StaticClass()))
	{
		m_AbilityEditor->SetOpen(true);
		m_AbilityEditor->SetTargetGameplayAbility((GameplayAbility*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsA(GameplayEffect::StaticClass()))
	{
		m_EffectEditor->SetOpen(true);
		m_EffectEditor->SetTargetGameplayEffect((GameplayEffect*)ClickedAsset);
	}
	else if(ClickedAsset->GetClass()->IsA(AnimationSequence::StaticClass()))
	{
		m_AnimationEditor->SetOpen(true);
		m_AnimationEditor->SetTargetAnimation((AnimationSequence*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsA(Sound::StaticClass()))
	{
		m_SoundEditor->SetOpen(true);
		m_SoundEditor->SetEditSound((Sound*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsChildOf(SceneInfo::StaticClass()) ||
			 ClickedAsset->GetClass()->IsA(SceneInfo::StaticClass()))
	{
		m_SceneInfoEditor->SetOpen(true);
		m_SceneInfoEditor->SetEditSceneInfo((SceneInfo*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsChildOf(Scene::StaticClass()) ||
			 ClickedAsset->GetClass()->IsA(Scene::StaticClass()))
	{		
		if (!g_Engine->Get<SceneManager>()->GetCurrentScene()->IsBegan())
			g_Engine->Get<SceneManager>()->CreateNextScene((Scene*)ClickedAsset);
	}
	else if(ClickedAsset->GetClass()->IsA(BehaviorTree::StaticClass()))
	{
		m_NodeEditor->SetOpen(true);
		m_NodeEditor->SetTargetBTree((BehaviorTree*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsA(Blackboard::StaticClass()))
	{
		m_BBEditor->SetOpen(true);
		m_BBEditor->SetEditBlackBoard((Blackboard*)ClickedAsset);
	}
	else if (ClickedAsset->GetClass()->IsChildOf(UserWidget::StaticClass()) ||
			 ClickedAsset->GetClass()->IsA(UserWidget::StaticClass()))
	{		
		m_WidgetEidtor->SetOpen(true);
		m_WidgetEidtor->SetTargetWidget((UserWidget*)ClickedAsset);
		
	}
	else if (ClickedAsset->GetClass()->IsA(ParticleAsset::StaticClass()))
	{
		m_ParticleEditor->SetOpen(true);
		m_ParticleEditor->SetTargetParticle((ParticleAsset*)ClickedAsset);
	}
	

}

void ContentEditor::Render()
{
	if(m_MeshEditor->IsOpened())
		m_MeshEditor->Render();

	if (m_ObjectEditor->IsOpened())
		m_ObjectEditor->Render();

	if (m_AnimationEditor->IsOpened())
		m_AnimationEditor->Render();

	if (m_WidgetEidtor->IsOpened())
		m_WidgetEidtor->Render();

	if (m_ParticleEditor->IsOpened())
		m_ParticleEditor->Render();
	//TargetViewer를 이용해서 랜더링이 필요한경우 이곳에서 랜더콜을합니다
}

void ContentEditor::PreRender()
{
	if (m_MeshEditor->IsOpened())
		m_MeshEditor->PreRender();

	if (m_ObjectEditor->IsOpened())
		m_ObjectEditor->PreRender();

	if (m_AnimationEditor->IsOpened())
		m_AnimationEditor->PreRender();

	if (m_WidgetEidtor->IsOpened())
		m_WidgetEidtor->PreRender();

	if (m_ParticleEditor->IsOpened())
		m_ParticleEditor->PreRender();
}
