#pragma once
#include "EditorWidget.h"

class PropertySet :public EditorWidget
{
	CLASS(PropertySet, EditorWidget)

	using FProperties = std::vector<EditorWidget*>;
	using FPropertyWidgetByGroup = std::unordered_map<std::string, FProperties>;

protected:
	PropertySet();
	virtual ~PropertySet();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

public:
	void SetPropertyTarget(class XObject* InPropertyTarget);

	XObject* GetPropertyTarget() const
	{
		return m_PropertyTarget;
	}

private:
	void UpdateChildProperties(float DeltaSeconds);
private:
	/* Property 를 가지고있는 클래스 */
	XObject* m_PropertyTarget;
	FPropertyWidgetByGroup	m_PropertiesByGroup;

};

