#pragma once
#include "EditorWidget.h"

/**
 * 만들수있는 모든 GameObject Class 를 계층구조 (TreeNode)로 표현합니다
 * m_TargetClass 는 범용성을 고려하여 만들었지만 GO이외는 사용할일이없을듯
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

