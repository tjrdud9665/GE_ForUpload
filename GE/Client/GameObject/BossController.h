#pragma once
#include <GameFramework\GameObject\AIController.h>

class BossController : public AIController
{
	CLASS(BossController, AIController)

protected:
	BossController();
	BossController(const BossController& Other);
	virtual ~BossController();

public:
	virtual void Update(float DeltaSeconds) override;

	virtual void BeginPlay() override;

private:
	void CheckPhase();

protected:

};

