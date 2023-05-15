#pragma once
#include "Widget.h"
class ProgressBar :public Widget
{
	CLASS(ProgressBar,Widget)

protected:
	ProgressBar();
	ProgressBar(const ProgressBar& Other);
	virtual ~ProgressBar();


	virtual void Render(class Renderer* pRenderer) override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

public:
	void SetPercent(float BarPecent);

	void BindPecent(std::function<float(void)> PercentCallback);

	void SetSpeed(float BarSpeed);


private:
	PROP(TSharedPtr<TextureAsset>, BackGroundImage, DetailsEdit)
	TSharedPtr<TextureAsset> BackGroundImage;

	PROP(TSharedPtr<TextureAsset>, FillImage, DetailsEdit)
	TSharedPtr<TextureAsset> FillImage;
	
	//Runtime
	float CurrentPercent;

	PROP(TEnum<EProgressBarDir>, BarDir, DetailsEdit)
	TEnum<EProgressBarDir>		BarDir;


	PROP(float, PercentTo, DetailsEdit)
	float					PercentTo;

	PROP(float, Speed, DetailsEdit)
	float					Speed;

	std::function<float(void)>		m_PercentCallback;

	std::vector<SVertex_PUV>	m_FillImageVertices;
};

