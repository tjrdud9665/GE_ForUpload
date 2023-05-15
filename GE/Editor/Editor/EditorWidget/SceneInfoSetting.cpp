#include "PCH.h"
#include "SceneInfoSetting.h"
#include "PropertySet.h"
#include <Assets/Scene/Scene.h>

SceneInfoSetting::SceneInfoSetting()
{

}

SceneInfoSetting::~SceneInfoSetting()
{

}

void SceneInfoSetting::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_ScenePropSet 	= AddNew<PropertySet>("ScenePropSet");

}

void SceneInfoSetting::SetCurrentScene(class Scene* InScene)
{
	m_CurrentScene = InScene;

	m_ScenePropSet->SetPropertyTarget(m_CurrentScene);
}
