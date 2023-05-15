#pragma once

#include "XObject/XObject.h"
#include "Editor.h"


/* Make Widget Base Class */

/**
 *							EditorManager
 *						Editor
 *					Widgets(Window)
 *				Widgets(NotWindow)
 */

class EditorManager : public XObject
{

	friend class EditorApp;
	friend class Globals;

private:
	EditorManager();
	virtual ~EditorManager();

private:
	void Initialize();

	virtual void Update(float DeltaSeconds) override;

	void UpdateDocking();
	
	void UpdateMainNavBar();

	void Render();
	void PreRender();


private:
	void RegisterPropertyDetails();


public:
	template<typename EditorType>
	EditorType* CreateEditor()
	{
		EditorType* createdEditor = Globals::CreateX<EditorType>(GetSelf());
		 m_Editors.push_back(createdEditor);
		 return createdEditor;		
	}

	template<typename EditorType>
	EditorType* FindEditor(const std::string& InName ) const
	{
		std::vector<TSharedPtr<Editor>>::const_iterator iter = m_Editors.begin();
		std::vector<TSharedPtr<Editor>>::const_iterator iter_end = m_Editors.end();

		for(;iter != iter_end;iter++)
		{
			if((*iter)->GetName() == InName)
			{
				return (EditorType*)((*iter).Get());
			}
		}
		return nullptr;
	}


private:
	std::vector<TSharedPtr<Editor>>			m_Editors;


private:
	bool m_EditorBegun;

	bool m_ShowDemo;


	std::string m_sceneName;

	//Capture Play.
	TSharedPtr<class Scene> m_PlayedScene;
	Json					m_PlayedSceneData;
};

