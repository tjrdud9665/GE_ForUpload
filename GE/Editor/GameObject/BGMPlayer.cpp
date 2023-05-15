#include "PCH.h"
#include "BGMPlayer.h"
#include "Assets/Sound/Sound.h"

BGMPlayer::BGMPlayer()
{

}

BGMPlayer::BGMPlayer(const BGMPlayer& Other)
	:Super(Other)
	, m_Bgm(Other.m_Bgm)
{
	if (IsConstructed())
	{
		m_Transform = FindComponent<SceneComponent>("Root");
	}
}

BGMPlayer::~BGMPlayer()
{
	if (m_Bgm)
		m_Bgm->Stop();
}

void BGMPlayer::Construct(const std::string& InName)
{
	Super::Construct(InName);

	m_Transform = CreateComponent<SceneComponent>("Root");

	SetRootComponent(m_Transform);
}

void BGMPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (m_Bgm)
	{
		m_Bgm->SetLoop(true);
		m_Bgm->Play();
	}
}