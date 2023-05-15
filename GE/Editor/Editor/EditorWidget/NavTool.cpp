#include "PCH.h"
#include "NavTool.h"
#include "NavToolWidget.h"
#include "PropertySet.h"
#include <Navigation/NavigationSystem.h>


NavTool::NavTool()
{

}

NavTool::~NavTool()
{

}

void NavTool::Build()
{
	Super::Build();
	m_NavToolWidget = EditorWidget::AddNew<NavToolWidget>("NavToolWidget");
	m_NavPropertySet = EditorWidget::AddNew<PropertySet>("NavPropSet");	
	m_NavPropertySet->SetPropertyTarget(g_Engine->Get<NavigationSystem>());
}
