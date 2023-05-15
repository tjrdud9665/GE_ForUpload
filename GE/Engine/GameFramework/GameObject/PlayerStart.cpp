#include "EnginePCH.h"
#include "PlayerStart.h"

PlayerStart::PlayerStart()
{

}

PlayerStart::PlayerStart(const PlayerStart& Other)
	:Super(Other)
{
	m_SceneRoot = FindComponent<SceneComponent>("SceneRoot");
}

PlayerStart::~PlayerStart()
{

}

void PlayerStart::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_SceneRoot = CreateComponent<SceneComponent>("SceneRoot");
	SetRootComponent(m_SceneRoot);

}

void PlayerStart::LocationUpdate()
{
	m_SceneRoot->ConditionalUpdateComponentToWorld();
}

void PlayerStart::BeginPlay()
{
	Super::BeginPlay();
}
