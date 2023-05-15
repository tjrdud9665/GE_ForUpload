#pragma once
#include "EditorWidget.h"

class TargetViewer :public EditorWidget
{
	CLASS(TargetViewer,EditorWidget)

protected:
	TargetViewer();
	virtual ~TargetViewer();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

public:
	void SetTargetTexID(FTextureID InTexID);

	void SetDesiredSizeRatioWidth(float Width);

	void SetDesiredSizeRatioHeight(float Height);


	const SVector2& GetTargetTexSize() const
	{
		return m_TargetTexSize;
	}

public:
	//CameraConroll Interface
	void SetCameraFocusMode(bool InFocusMode);

	void SetCameraFocusLocation(const SVector3& InLocation);

private:
	class Renderer*			m_cachedRenderer;
	class RenderManager*	m_cahcedRenderManager;
	FTextureID				m_TagetTexID;

	SVector2				m_TargetTexSize;

	SVector2				m_DesiredSizeRatio;

	class CameraControlWidget* m_CameraController;
};

