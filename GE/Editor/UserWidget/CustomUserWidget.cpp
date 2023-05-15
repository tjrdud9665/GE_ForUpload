#include "PCH.h"
#include "CustomUserWidget.h"
#include <Assets/Widget/ButtonWidget.h>
#include <Assets/Widget/WidgetTree.h>
#include <Assets/Widget/SizeBox.h>
#include <Assets/Widget/Overlay.h>

//*** C++ 에서 추가한 위젯을 에디터에서 지우지마세요 * >원래는 래거시 값을줘서 삭제를 막아야하나
//시간이없음



CustomUserWidget::CustomUserWidget()
{

}

CustomUserWidget::CustomUserWidget(const CustomUserWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{
		m_Button = (ButtonWidget*)m_WidgetTree->FindWidget("Button_1");
		m_RootSizeBox = (SizeBox*)m_WidgetTree->FindWidget("SizeBox_1");
		m_ButtonOverlay = (Overlay*)m_WidgetTree->FindWidget("Overlay_1");
	}
	//SizeBox_1
	//"Button_1"
}

CustomUserWidget::~CustomUserWidget()
{

}

void CustomUserWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
	m_RootSizeBox = m_WidgetTree->ConstructWidget<SizeBox>(SizeBox::StaticClass(), "SizeBox");
	m_Button = m_WidgetTree->ConstructWidget<ButtonWidget>(ButtonWidget::StaticClass(),"Button");	
	m_ButtonOverlay = m_WidgetTree->ConstructWidget<Overlay>(Overlay::StaticClass(), "Overlay");
	
	
	m_WidgetTree->SetRootWidget(m_RootSizeBox);
	m_RootSizeBox->AddChild(m_Button);
	m_Button->AddChild(m_ButtonOverlay);



}

void CustomUserWidget::BeginPlay()
{
	Super::BeginPlay();

	m_Button->BindOnPressed(this, &CustomUserWidget::OnButtonPressed);
}

void CustomUserWidget::OnButtonPressed()
{
	int32 t = 0;
}
