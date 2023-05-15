#pragma once
#include "GameFramework\GameObject\GameObject.h"

class BGMPlayer : public GameObject
{
	CLASS(BGMPlayer, GameObject)

protected:
	BGMPlayer();
	BGMPlayer(const BGMPlayer& Other);
	virtual ~BGMPlayer();

protected:
	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;

private:
	TSharedPtr<class SceneComponent>        m_Transform;

	PROP(TSharedPtr<class Sound>, m_Bgm, DetailsEdit)
		TSharedPtr<class Sound>            m_Bgm;
};

