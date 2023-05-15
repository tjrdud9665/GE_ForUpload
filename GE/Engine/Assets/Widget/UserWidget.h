#pragma once
#include "Widget.h"


class UserWidget :public Widget	
{
	CLASS(UserWidget, Widget)
	friend class WidgetTree;
	friend class WidgetEditor;
	friend class SceneViewport;

protected:
	UserWidget();
	UserWidget(const UserWidget& Other);
	virtual ~UserWidget();

	virtual void BeginPlay() override;

	virtual void SetOwningScene(class Scene* InScene) override;

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual bool PostLoad() override;

	virtual void PreUpdate(float DeltaSeconds) override;
	
	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual Widget* CollisionMouse(class Input* pInput);

	virtual void SetClipSpace(const SScissorsRect& InRect) override;

public:
	virtual void Render(class Renderer* pRenderer) override;

	virtual bool Serialize(Archive& ar) override;

public:
	bool IsVisible()	const;
	void SetVisible(bool visible);

public:
	class WidgetTree* GetWidgetTree() const;
	
	TSharedPtr<WidgetTree> m_WidgetTree;	
	
	
};

