#pragma once
#include "PropertyWidget.h"

/* SceneComponent를 상속받은 모든 컴포넌트의 Property 를 표현합니다. 
* 기존 PropertySet로 표현하던것을 PropertWidget으로 표현해서 루트의 모든항목을 GameOBject의 항목에 같이 표기합니다.
*/

class SceneComponentWidget :public PropertyWidget
{
	CLASS(SceneComponentWidget, PropertyWidget)

	using FProperties = std::vector<EditorWidget*>;
	using FPropertyWidgetByGroup = std::unordered_map<std::string, FProperties>;


protected:
	SceneComponentWidget();
	virtual ~SceneComponentWidget();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	TPropertyHandle<TSharedPtr<class SceneComponent>>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<TSharedPtr<class SceneComponent>>*>(m_Prop);
	}

private:

	void UpdateChildProperties(float DeltaSeconds);

	FPropertyWidgetByGroup	m_PropertiesByGroup;
};

