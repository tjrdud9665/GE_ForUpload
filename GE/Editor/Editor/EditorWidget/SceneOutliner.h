#pragma once
#include "EditorWindow.h"

/* ���� Scene �� ������ ������Ʈ�� ����� �����ݴϴ�. */
/* ������Ʈ�� ���������� ����ؼ� ListBox �ȿ� TreeNode�� �����մϴ�.*/


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

