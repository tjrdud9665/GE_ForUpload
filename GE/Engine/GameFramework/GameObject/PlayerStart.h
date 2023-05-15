#pragma once
#include "GameObject.h"

/**
 * Scene �� ���۵� �� �ش� ��ġ�� SceneInfo���� ������ Ŭ������ Character . (Pawn)�� �����մϴ�.
 * Player Character �� ������ġ�Դϴ�.
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

