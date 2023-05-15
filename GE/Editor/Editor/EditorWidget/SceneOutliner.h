#pragma once
#include "EditorWindow.h"

/* 현재 Scene 에 생성된 오브젝트의 목록을 보여줍니다. */
/* 오브젝트간 계층구조를 고려해서 ListBox 안에 TreeNode를 구성합니다.*/


class SceneOutliner :public EditorWindow
{
	CLASS(SceneOutliner, EditorWindow)

protected:
	SceneOutliner();
	SceneOutliner(const SceneOutliner& Other);
	virtual ~SceneOutliner();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

public:
	void SetPickedComponentObject(class GameObject* InObject);

private:
	void BuildObjectTree();
	void BuildObjectSubTree(class GameObject* ParentGameObject);

	void HandleClick();


private:
	void	AddSelectedList(class GameObject* InObject);

	void	ClearSlectedList();

	bool	HasSelectedList(class GameObject* InObject) const;


private:
	class SceneManager* m_cachedSceneManager;
	std::list<TSharedPtr< class GameObject>>		m_SelectedList;

	class GameObject*				m_Hovered;
	class GameObject*				m_Clicked;

	bool							m_ShowContextPop;
};

