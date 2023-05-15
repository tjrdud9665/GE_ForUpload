#include "PCH.h"
#include "NavToolWidget.h"
#include "imgui.h"
#include <Navigation/NavigationSystem.h>

NavToolWidget::NavToolWidget()
{

}

NavToolWidget::~NavToolWidget()
{

}

void NavToolWidget::Update(float DeltaSeconds)
{
	//SetUp Nav Generation Options..

	//Save To Scene..


	if(ImGui::Button("Generate NavMesh"))
	{
		m_cahcedNavSystem->BuildTest();
	}
}

void NavToolWidget::Build()
{
	Super::Build();
	m_cahcedNavSystem = g_Engine->Get<NavigationSystem>();

}
