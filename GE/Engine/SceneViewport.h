#pragma once
#include "XObject/XObject.h"
#include "Assets/Scene/Scene.h"

class SceneViewport :public XObject
{
	friend class Scene;
	friend class Globals;

private:
	SceneViewport();
	virtual ~SceneViewport();

public:
	bool Initialize(class Scene* InScene);

	void BeginPlay();

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void PreUpdate(float DeltaSeconds) override;

	void Render(class Renderer* pRenderer);

	void EndPlay();
	

	template<typename UserWidgetType, typename = std::enable_if_t<std::is_base_of<UserWidget, UserWidgetType>::value>>	
	UserWidgetType* CreateUserWidget(TSubclassOf<UserWidgetType> WidgetClass = UserWidgetType::StaticClass(), const std::string& WidgetName = "")
	{
		std::string widgetName = WidgetName;
		if (widgetName.empty())
		{
			
			widgetName = WidgetClass.CustomCDOName;
				//WidgetClass->GetName();

		}

		stringstream ss;
		auto iter = m_InstanceCounter.find(widgetName);

		if (iter == m_InstanceCounter.end())
			m_InstanceCounter.insert(std::make_pair(widgetName, 0));

		ss << widgetName << "_" << ++m_InstanceCounter[widgetName];

		UserWidgetType* widget = Globals::CreateX<UserWidgetType>(WidgetClass, this, ss.str());

		widget->SetOwningScene(m_OwningScene);

		if(m_OwningScene->IsBegan())
		{
			widget->BeginPlay();
		}

		m_vecWidget.push_back(widget);

		return widget;		
	}

protected:
	Scene* m_OwningScene;


	//ViewPortWidget..
	std::vector<TSharedPtr<class UserWidget>>		m_vecWidget;

	TSharedPtr<class Widget>						m_CollisionWidget;




private:	
	std::unordered_map<std::string, uint64>	m_InstanceCounter;


	//std::unordered_map<std::string, uint64>	m_InstanceCounter;
	bool										m_prevMRB;
	bool										m_prevMLB;


	TSharedPtr<class Widget>					m_PressedWidget;
	//Shared ptr·Î 

private:
	static bool SortViewprotWidget(TSharedPtr<class Widget> Src, TSharedPtr<class Widget> Dest);
	static bool SortViewprotWidgetInv(TSharedPtr<class Widget> Src, TSharedPtr<class Widget> Dest);


	class Input* m_cahcedInput;

	class UIManager* m_CachedManager;
};

