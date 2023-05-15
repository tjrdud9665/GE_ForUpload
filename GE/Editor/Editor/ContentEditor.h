#pragma once
#include "Editor.h"

/* 
* 에디터 내에서 한글 사용 x
*/
class ContentEditor :public Editor
{
	CLASS(ContentEditor, Editor)

protected:
	ContentEditor();
	ContentEditor(const ContentEditor& Other);
	virtual ~ContentEditor();

public:
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void ExpandViewsMenu() override;

	virtual void Update(float DeltaSeconds) override;

	void OnClickedAsset(class AssetData* ClickedAsset);

	virtual void Render() override;

	virtual void PreRender() override;


private:
	TSharedPtr<class AssetViewer>		m_AssetViewer;

	TSharedPtr<class MaterialEditor>	m_MaterialEditor;

	TSharedPtr<class MeshEditor>		m_MeshEditor;

	TSharedPtr<class GameObjectEditor>	m_ObjectEditor;

	TSharedPtr<class GameplayAbilityEditor>	m_AbilityEditor;

	TSharedPtr<class GameplayEffectEditor>	m_EffectEditor;

	TSharedPtr<class SceneInfoEditor>	m_SceneInfoEditor;

	TSharedPtr<class AnimationEditor>	m_AnimationEditor;

	TSharedPtr<class SoundEditor>		m_SoundEditor;


	///Ai

	TSharedPtr<class NodeEditor>		m_NodeEditor;

	TSharedPtr<class BlackboardEditor>		m_BBEditor;

	TSharedPtr<class WidgetEditor>			m_WidgetEidtor;

	TSharedPtr<class ParticleEditor>		m_ParticleEditor;

};

