#pragma once

#include "EditorWidget.h"

class CameraControlWidget :public EditorWidget
{
	CLASS(CameraControlWidget, EditorWidget)

public:
	CameraControlWidget();	
	virtual ~CameraControlWidget();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	void SetFocusMode(bool focusMode);
	
	void SetFocusLocation(const SVector3& InLocation);


private:
	class EditorWindow* m_TargetWindow;

	float				m_CameraMoveSpeed;

	bool				m_FocusMode;

	SVector3			m_WorldFoucsLocation;


};

