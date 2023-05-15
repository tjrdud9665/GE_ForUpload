#include "PCH.h"
#include "TestCharacter.h"
#include <Assets/Mesh/Skeleton.h>
#include <Assets/Animation/AnimationSequence.h>
#include <GameFramework/Components/CharacterMovementComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/StaticMeshComponent.h>
#include <CameraManager.h>
#include <GameFramework/Components/CameraComponent.h>



TestCharacter::TestCharacter()
{
}

TestCharacter::TestCharacter(const TestCharacter& Other)
	:Super(Other)
	, TestSkiil(Other.TestSkiil)
	, TestSkiil2(Other.TestSkiil2)
{
	WeaponTest = FindComponent<StaticMeshComponent>("WeaponTest");
}

TestCharacter::~TestCharacter()
{

}

void TestCharacter::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	WeaponTest = CreateComponent<StaticMeshComponent>("WeaponTest");

	GetSkeletalMeshComponent()->AddChild(WeaponTest);

}

void TestCharacter::SetUpInput(InputHandle& handle)
{
	Super::SetUpInput(handle);

	handle.AddAxisBind("MoveForward", std::bind(&TestCharacter::MoveForward, this, std::placeholders::_1));

	handle.AddAxisBind("MoveRight", std::bind(&TestCharacter::MoveRight, this, std::placeholders::_1));

	handle.AddActionBind("MRB", EInputEventType::IE_Pressed, std::bind(&TestCharacter::MouseClicked, this, std::placeholders::_1, std::placeholders::_2));
	//can From Ability
	//handle.AddActionBind("Test", EInputEventType::IE_Pressed,std::bind(&TestCharacter::TestAttack, this));


}

void TestCharacter::CheckTagStatement()
{
	bool DesiredMovement = false;
	bool WantIdle = true;
	bool ActivatingSomeAbility = false;

	//이동 마지막에 느려지는것

	if (GetMovementComponent()->IsPathFollowing())
	{
		DesiredMovement = true;
	}
	//임시 코드
	//활성중인 스킬일 하나라도 있으면 
	//Walk Idle을막는다
	if (GetAbilityComponent()->GetActivatingAbility())
	{
		WantIdle = false;
		ActivatingSomeAbility = true;
	}


	//Temp Code.
	if (!ActivatingSomeAbility)
	{
		if (DesiredMovement)
		{
			GetSkeletalMeshComponent()->SetAnimationSequence(Walk);

		}
		else if (WantIdle)
		{
			GetSkeletalMeshComponent()->SetAnimationSequence(Idle);
		}
		if (!GetSkeletalMeshComponent()->IsReversing())
			GetSkeletalMeshComponent()->Play();

		GetSkeletalMeshComponent()->SetLooping(true);
		//GetSkeletalMeshComponent()->SetEnableRootMotion(false);

	}

}

void TestCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_AbilityComponent->GiveAbility(TestSkiil);
	m_AbilityComponent->GiveAbility(TestSkiil2);


}

void TestCharacter::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void TestCharacter::MoveForward(float Axis)
{
	GetMovementComponent()->AddInputVector(SVector3::ForwardVector * Axis);

}

void TestCharacter::MoveRight(float Axis)
{
	GetMovementComponent()->AddInputVector(SVector3::RightVector * Axis);

}


void TestCharacter::MouseClicked(const std::string& BindName, uint64 ID)
{
	D3D11_VIEWPORT viewport = g_Engine->Get<Renderer>()->GetCurrentViewport();

	if (g_Engine->IsEditMode())
	{
		viewport = g_Engine->GetEditViewport();
	}

	//viewport.Height = winSize.y;//ImGui::GetWindowViewport()->Size.y;
	//viewport.Width = winSize.x;//winSize.x;//ImGui::GetWindowViewport()->Size.x;	
	//viewport.TopLeftX = winPos.x;
	//viewport.TopLeftY = winPos.y;

	//ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
	//if (dockNode)
	//{
	//	viewport.Width = dockNode->Size.x;
	//	viewport.Height = dockNode->Size.y;
	//	viewport.TopLeftX = dockNode->Pos.x;
	//	viewport.TopLeftY = -dockNode->Pos.y;

	//}

	SVector2 mouse = g_Engine->Get<Input>()->GetMousePosition();

	SVector3 origin;
	SVector3 direction;

	SHitResult hit = g_Engine->Get<CameraManager>()->GetCurrentCamera()->pickRay(viewport, mouse.x, mouse.y, OUT origin, OUT direction);

	if (hit.Hit)
	{
		SVector3 hitLoc = hit.point;

		GetMovementComponent()->MoveToTargetLocation(hitLoc);
	}

}

