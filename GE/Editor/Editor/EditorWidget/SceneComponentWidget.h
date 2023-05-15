#pragma once
#include "PropertyWidget.h"

/* SceneComponent�� ��ӹ��� ��� ������Ʈ�� Property �� ǥ���մϴ�. 
* ���� PropertySet�� ǥ���ϴ����� PropertWidget���� ǥ���ؼ� ��Ʈ�� ����׸��� GameOBject�� �׸� ���� ǥ���մϴ�.
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

