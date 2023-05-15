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



	//���� �迭�� ����ؼ� ����Ұ�� �Է� ���ε��� ���ؼ� 
	//MapPropert�� �����̾ȵ�����
	//���ε��� ���� �迭�� �ʿ�	
	PROP(std::vector<TSubclassOf<class GameplayAbility>>, TestSkillArr, DetailsEdit, "SkiilTest")
		std::vector<TSubclassOf<class GameplayAbility>> TestSkillArr;

	//�迭��  ������


	class StaticMeshComponent* WeaponTest;

};

