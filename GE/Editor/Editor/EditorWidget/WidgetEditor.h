#pragma once
#include "EditorWindow.h"
# include <imgui.h>
# define IMGUI_DEFINE_MATH_OPERATORS
# include <imgui_internal.h>
# include <imgui_canvas.h>

class WidgetEditor :public EditorWindow
{
	CLASS(WidgetEditor,EditorWindow)

protected:
	WidgetEditor();
	virtual ~WidgetEditor();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	void DrawCanvas(float DeltaSeconds);

	void DrawWidgetsInCanvas(class WidgetTree* widgetTree);
		
	void ShowMenu();


	bool ShowWidgetTree(class WidgetTree* widgetTree, class Widget* InParet);

public:
	void SetTargetWidget(class UserWidget* InTargetWidget);

	void Render();

	void PreRender();

private:
	void HandleClicked();
	void ShowContextPop();



protected:
	ImGuiEx::Canvas m_canvas;

	class UserWidget* m_TargetWidet;

	bool			m_PaletteDrag;
	
	std::vector<XClass*>	m_panelWidgetClass;

	std::vector<XClass*>	m_imageWidgetClass;

	std::vector<XClass*>	m_textWidgetClass;

	std::vector<XClass*>	m_conentWidgetClass;	//	
	


	std::vector<TSubclassOf<UserWidget>>		m_userWidgetClass;

	class PropertySet* m_widgetDetails;
	
	class PropertySet* m_slotDetails;

	class Widget* m_Hovered;

	class Widget* m_Clicked;
	

	bool m_ShowWidgetContextPop;


};

