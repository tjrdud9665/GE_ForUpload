#pragma once

#include "../AssetData.h"
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/GameObject/PlayerController.h>
#include "../Widget/UserWidget.h"

class SceneInfo :public AssetData
{
	CLASS(SceneInfo, AssetData)
	friend class Scene;

protected:
	SceneInfo();
	SceneInfo(const SceneInfo& Other);
	virtual ~SceneInfo();


	virtual bool PostLoad() override;

protected:
	virtual void BeginPlay();

	virtual void EndPlay();


	virtual void PreUpdate(float DeltaSeconds);

	virtual void Update(float DeltaSeconds);

	virtual void PostUpdate(float DeltaSeconds);


public:
	class Character* GetPlayerCharacter()const;

	class PlayerController* GetPlayerController() const;

	virtual SVector3	GetPlayerStartLocation() const;


private:
	PROP(TSubclassOf<UserWidget>, m_DefaultWidgetClass, DetailsEdit)
	TSubclassOf<UserWidget>		m_DefaultWidgetClass;

	PROP(TSubclassOf<Character>, m_DefaultCharacterClass, DetailsEdit)
		TSubclassOf<Character> m_DefaultCharacterClass;

	//PlayerController
	PROP(TSubclassOf<PlayerController>, m_DefaultControllerClass, DetailsEdit)
		TSubclassOf<PlayerController> m_DefaultControllerClass;




	class Scene* m_OwningScene;

	TSharedPtr<Character> m_PlayerCharacter;
	PlayerController* m_PlayerController;

public:
	virtual void SetOwner(XObject* InOwner) override;
};

