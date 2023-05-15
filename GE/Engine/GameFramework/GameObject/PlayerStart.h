#pragma once
#include "GameObject.h"

/**
 * Scene 이 시작될 때 해당 위치에 SceneInfo에서 설정된 클래스로 Character . (Pawn)를 스폰합니다.
 * Player Character 의 스폰위치입니다.
 */

class PlayerStart : public GameObject
{
	CLASS(PlayerStart , GameObject)

protected:
	PlayerStart();
	PlayerStart(const PlayerStart& Other);
	virtual ~PlayerStart();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void BeginPlay() override;

private:
	class SceneComponent* m_SceneRoot;

public:
	void LocationUpdate();
};

