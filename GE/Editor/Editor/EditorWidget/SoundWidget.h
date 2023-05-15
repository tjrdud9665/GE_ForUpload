#pragma once
#include "EditorWidget.h"

class SoundWidget : public EditorWidget
{
	CLASS(SoundWidget, EditorWidget)

public:
	SoundWidget();
	SoundWidget(const SoundWidget& Other);
	virtual ~SoundWidget();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	void SetEditSound(class Sound* InSound);

private:
	class Sound* m_EditSound;

	class SoundManager* m_cachedMgr;
};

