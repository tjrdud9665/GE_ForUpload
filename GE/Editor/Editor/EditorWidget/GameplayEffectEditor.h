#pragma once
#include "EditorWindow.h"

class GameplayEffectEditor :public EditorWindow
{
	CLASS(GameplayEffectEditor,EditorWindow)

public:
	GameplayEffectEditor();
	GameplayEffectEditor(const GameplayEffectEditor& Other);
	virtual ~GameplayEffectEditor();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	void ShowMenu();

	void SetTargetGameplayEffect(class GameplayEffect* InGameplayEffect);

private:
	class GameplayEffect* m_EditEffect;

	class PropertySet* m_EffectProperty;
};

