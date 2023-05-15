#include "EnginePCH.h"
#include "ProgressBar.h"

ProgressBar::ProgressBar()
	:CurrentPercent(1.0f)
	, BarDir(EProgressBarDir::RightToLeft)
	, PercentTo(1.0f)
	, Speed(2.5f)
{

}

ProgressBar::ProgressBar(const ProgressBar& Other)
	:Super(Other)
	, CurrentPercent(Other.CurrentPercent)
	, BarDir(Other.BarDir)
	, PercentTo(Other.PercentTo)
	, Speed(Other.Speed)
	, BackGroundImage(Other.BackGroundImage)
	, FillImage(Other.FillImage)
{

}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::Render(class Renderer* pRenderer)
{
	//L		R
	//1		2
	//
	//0
	// 
	//		4
	//
	//3		5
	if (Slot)
	{
		if (BackGroundImage)
		{
			m_CachedManager->PushSlot(Slot, BackGroundImage->GetTextureID());			

			if(FillImage)
			{
				m_FillImageVertices = Slot->GetGeometryVertices();
				if (BarDir.Get() == EProgressBarDir::RightToLeft)
				{
					float def = m_FillImageVertices[2].Position.x - m_FillImageVertices[1].Position.x;
					//2 4 5 		
					float  adder = CurrentPercent* def;

					m_FillImageVertices[2].Position.x = m_FillImageVertices[1].Position.x + adder;
					m_FillImageVertices[4].Position.x = m_FillImageVertices[1].Position.x + adder;
					m_FillImageVertices[5].Position.x = m_FillImageVertices[1].Position.x + adder;
				}
				else if (BarDir.Get() == EProgressBarDir::LeftToRight)
				{
					float def = m_FillImageVertices[2].Position.x - m_FillImageVertices[1].Position.x;
					float  adder = CurrentPercent * def;
					//0 1 3
					m_FillImageVertices[0].Position.x = m_FillImageVertices[2].Position.x - adder;
					m_FillImageVertices[1].Position.x = m_FillImageVertices[2].Position.x - adder;
					m_FillImageVertices[3].Position.x = m_FillImageVertices[2].Position.x - adder;

				}
				else if (BarDir.Get() == EProgressBarDir::TopToBottom)
				{
					float def = m_FillImageVertices[2].Position.y - m_FillImageVertices[5].Position.y;
					float  adder = CurrentPercent * def;

					//1 2 4
					m_FillImageVertices[1].Position.y = m_FillImageVertices[5].Position.y + adder;
					m_FillImageVertices[2].Position.y = m_FillImageVertices[5].Position.y + adder;
					m_FillImageVertices[4].Position.y = m_FillImageVertices[5].Position.y + adder;
				}
				else if (BarDir.Get() == EProgressBarDir::BottomToTop)
				{
					//0 3 5
					float def = m_FillImageVertices[2].Position.y - m_FillImageVertices[5].Position.y;
					float  adder = CurrentPercent * def;

				
					m_FillImageVertices[0].Position.y = m_FillImageVertices[2].Position.y - adder;
					m_FillImageVertices[3].Position.y = m_FillImageVertices[2].Position.y - adder;
					m_FillImageVertices[5].Position.y = m_FillImageVertices[2].Position.y - adder;
				}

				SVector4 contentColor = SVector4(TintColor.ToVector3(), RenderOpacity);
				m_CachedManager->PushTexturedTri(FillImage->GetTextureID(), contentColor, m_FillImageVertices.data(), 6);
			}
							

			D3DTexture* texObj = pRenderer->GetTextureObject(BackGroundImage->GetTextureID());
			DesiredSize = SVector2(texObj->GetWidth(), texObj->GetHeight());
		} 
		else
		{
			m_CachedManager->PushSlot(Slot, INDEX_NONE);
		}

	}
}

void ProgressBar::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (m_PercentCallback)
	{
		SetPercent(m_PercentCallback());
	}
}

void ProgressBar::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	CurrentPercent = SMathUtils::InterpTo(CurrentPercent, PercentTo, DeltaSeconds, Speed);
}

void ProgressBar::SetPercent(float BarPecent)
{
	PercentTo = SMathUtils::Clamp<float>(BarPecent, 0.0f, 1.0f);
}

void ProgressBar::BindPecent(std::function<float(void)> PercentCallback)
{
	m_PercentCallback = PercentCallback;
}
