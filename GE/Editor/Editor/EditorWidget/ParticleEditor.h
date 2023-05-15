#pragma once

#include "EditorWindow.h"


class ParticleEditor :public EditorWindow
{
	CLASS(ParticleEditor, EditorWindow)

public:
	ParticleEditor();	
	ParticleEditor(const ParticleEditor& Other);
	virtual ~ParticleEditor();

	virtual void Build() override;

	void SetTargetParticle(class ParticleAsset* InParticle);

	virtual void Update(float DeltaSeconds) override;

	virtual void Construct(const std::string& InName = "") override;

	void ShowParticleDetails(float DeltaSeconds);

	void Render();

	void PreRender();

	void ShowMenu();

	void UpdateEditSub(float DeltaSeconds);

	void ComposePreview();

	void TargetViewHandleInput();

private:	
	class PropertySet* m_ParticlePropertySet;
	class TargetViewer* m_Viewer;

	class ParticleAsset* m_TargetParticle;

	class Renderer* m_cachedRenderer;	

	class CameraManager* m_ImitateCameraManager;
	class RenderManager* m_ImitateRenderManager;
	TSharedPtr<class ParticleComponent>	m_ParticlePreviewComponent;



	float		m_TargetViewRatioX;

	float		m_PropPosY;
	float		m_PropPosX;
};

