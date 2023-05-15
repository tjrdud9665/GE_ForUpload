#include "PCH.h"
#include "EditorWidget.h"
#include "imgui.h"
#include "imgui_internal.h"

EditorWidget::EditorWidget()
	:m_Built(false)
	, m_Building(false)
	, m_DisableFlag(false)
	, m_WithSameLine(false)
{

}

EditorWidget::~EditorWidget()
{

}

void EditorWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

}

EditorWidget* EditorWidget::FindWidget(const std::string& InName) const
{
	FWidgetContainer::const_iterator iter = m_Children.begin();
	FWidgetContainer::const_iterator iter_end = m_Children.end();

	for(;iter != iter_end;iter++)
	{
		if ((*iter)->GetName() == InName)
			return (*iter);
	}

	return nullptr;
}

void EditorWidget::OnBuild()
{
	if (!m_Built && !m_Building)
	{
		m_Building = true;
		Build();
		m_Building = false;
		m_Built = true;

		FWidgetContainer::iterator iter = m_Children.begin();
		FWidgetContainer::iterator iter_end = m_Children.end();

		for(;iter != iter_end;iter++)
		{
			(*iter)->OnBuild();
		}
	}
}

void EditorWidget::OnUpdate(float DeltaSeconds)
{
	if(m_Built)
	{
		const bool bDisabledNow = m_DisableFlag;

		if (bDisabledNow)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		Update(DeltaSeconds);
		if (bDisabledNow)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

	}
}

void EditorWidget::ForceRebuild()
{
	if (!m_Building)
	{
		m_Built = false;
		m_Children.clear();
		OnBuild();
	}
}

void EditorWidget::Update(float DeltaSeconds)
{
	UpdateChilds(DeltaSeconds);
}

void EditorWidget::SetWithSameline(bool InSameLine)
{
	m_WithSameLine = InSameLine;
}

void EditorWidget::UpdateChilds(float DeltaSeconds)
{
	FWidgetContainer::iterator iter = m_Children.begin();
	FWidgetContainer::iterator iter_end = m_Children.end();

	for(;iter != iter_end;iter++)
	{
		(*iter)->OnUpdate(DeltaSeconds);
		if((*iter)->m_WithSameLine)
			ImGui::SameLine();
	}
}

void EditorWidget::SetDisableFlag(bool Disable)
{
	m_DisableFlag = Disable;
}

void EditorWidget::RemoveAllChildren()
{
	m_Children.clear();
}


void EditorWidget::RemoveWidget(class EditorWidget* InRemoval)
{
	FWidgetContainer::iterator iter = m_Children.begin();
	FWidgetContainer::iterator End = m_Children.end();
	for (; iter != End; iter++)
	{
		if ((*iter) == InRemoval)
		{
			m_Children.erase(iter);
			break;
		}
	}
}
