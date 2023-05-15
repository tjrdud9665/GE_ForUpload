#pragma once
#include "EditorWindow.h"

class AnimationEditor :public EditorWindow
{
	CLASS(AnimationEditor, EditorWindow)

public:
	AnimationEditor();
	AnimationEditor(const AnimationEditor& Other);
	virtual ~AnimationEditor();

	virtual void Build() override;

	void SetTargetAnimation(class AnimationSequence* AnimSeq);

	virtual void Update(float DeltaSeconds) override;

	virtual void Construct(const std::string& InName = "") override;	

	void Render();

	void PreRender();

	void ShowMenu();

	void UpdateEditSub(float DeltaSeconds);

	void ComposePreview();

	void ShowAnimationSequencer(float DeltaSeconds);

	void TargetViewHandleInput();

private:
	class AnimationSequencer* m_Sequencer;
	class PropertySet* m_NotifyInfoPropertySet;



	class TargetViewer* m_Viewer;

	class AnimationSequence* m_TargetAnimSeq;

	class Renderer* m_cachedRenderer;	

	class AssetManager* m_cachedAssetManager;

	class CameraManager* m_ImitateCameraManager;
	class RenderManager* m_ImitateRenderManager;	
		
	TSharedPtr<class SkeletalMeshComponent>	m_AnimPreviewComponent;
	TSharedPtr<class SkyComponent>		m_PreviewSky;

	TSharedPtr<class DirectionalLightComponent>	m_PreviewDirectionalLight;

	
	float		m_TargetViewRatioX;

	float		m_PropPosY;
	float		m_PropPosX;


	
	

};

