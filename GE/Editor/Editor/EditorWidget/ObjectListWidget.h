#pragma once
#include "EditorWidget.h"

/**
 * ������ִ� ��� GameObject Class �� �������� (TreeNode)�� ǥ���մϴ�
 * m_TargetClass �� ���뼺�� ����Ͽ� ��������� GO�ܴ̿� ��������̾�����
 */

class ObjectListWidget :public EditorWidget
{
	CLASS(ObjectListWidget, EditorWidget)

public:
	ObjectListWidget();
	virtual ~ObjectListWidget();


	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Update(float DeltaSeconds) override;

	TSubclassOf<class GameObject> GetClickedClass() const;

private:
	void BuildGameObjectHierachy(TSubclassOf<class GameObject> InParent);

	void HandleClick();
private:
	TSubclassOf<class GameObject> m_TargetClass;
	
	TSubclassOf<class GameObject>	m_Hovered;

	TSubclassOf<class GameObject>	m_Clicked;
	
	

};

