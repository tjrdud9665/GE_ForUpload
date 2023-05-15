#pragma once

#include "EditorWindow.h"
#include "Assets/AI/BlackBoard.h"

class BlackboardEditor :public EditorWindow
{
	CLASS(BlackboardEditor,EditorWindow)

protected:
	BlackboardEditor();
	virtual ~BlackboardEditor();

	virtual void Build() override;



	virtual void Update(float DeltaSeconds) override;


	void UpdateBlackboardEditing();

	void ShowMenu();

public:
	void SetEditBlackBoard(class Blackboard* InBB);

private:
	 Blackboard* m_EditingBlackBoard;
	 
	
private:
	EBlackBoardValType	m_NewBBKeyType;

	std::string			m_newKeyName;

	EBlackBoardValType	m_SelectedKeyType;

	std::string			m_selectedKeyName;

};

