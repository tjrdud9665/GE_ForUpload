#pragma once
#include "EditorWindow.h"

class GameplayAbilityEditor :public EditorWindow
{
	CLASS(GameplayAbilityEditor, EditorWindow)

public:
	GameplayAbilityEditor();
	GameplayAbilityEditor(const GameplayAbilityEditor& Other);
	virtual ~GameplayAbilityEditor();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;
	
	void ShowMenu();

	void SetTargetGameplayAbility(class GameplayAbility* InGameplayAbility);

private:
	class GameplayAbility* m_EditAbility;

	class PropertySet* m_AbilityProperty;

};

