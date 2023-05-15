#pragma once
#include <GameFramework/GameObject/PlayerCharacter.h>


class TestCharacter :public PlayerCharacter
{
	CLASS(TestCharacter, PlayerCharacter)
protected:
	TestCharacter();
	TestCharacter(const TestCharacter& Other);
	virtual ~TestCharacter();


protected:
	virtual void Construct(const std::string& InName = "") override;

	virtual void SetUpInput(InputHandle& handle) override;

	virtual void CheckTagStatement() override;

	virtual void BeginPlay() override;

	virtual void Update(float DeltaSeconds) override;

private:
	void MoveForward(float Axis);

	void MoveRight(float Axis);

	void MouseClicked(const std::string& BindName, uint64 ID);


	PROP(TSubclassOf<class GameplayAbility>, TestSkiil, DetailsEdit, "SkiilTest")
		TSubclassOf<class GameplayAbility>	TestSkiil;

	PROP(TSubclassOf<class GameplayAbility>, TestSkiil2, DetailsEdit, "SkiilTest")
		TSubclassOf<class GameplayAbility>	TestSkiil2;



	//만약 배열로 등록해서 사용할경우 입력 바인딩을 위해서 
	//MapPropert가 지원이안됨으로
	//바인딩을 위한 배열이 필요	
	PROP(std::vector<TSubclassOf<class GameplayAbility>>, TestSkillArr, DetailsEdit, "SkiilTest")
		std::vector<TSubclassOf<class GameplayAbility>> TestSkillArr;

	//배열로  가능함


	class StaticMeshComponent* WeaponTest;

};

