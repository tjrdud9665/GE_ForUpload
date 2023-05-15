#include "PCH.h"
#include "BossController.h"

BossController::BossController()
{
}

BossController::BossController(const BossController& Other)
	: Super(Other)
{
}

BossController::~BossController()
{
}

void BossController::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);


}

void BossController::BeginPlay()
{
	Super::BeginPlay();


}

void BossController::CheckPhase()
{

}
