#pragma once

#include "../../XObject/XObject.h"
#include "../../Timer/TimerHandle.h"

/**
 *  Component �� ��� Ŭ�����Դϴ�. 
 *  �п������� �°� ������Ʈ�� �������� ���������� ������ sceneComponent �� ������ ������ ObjectComponet�� �����ϴ�.
 *  Component �� ���� �������� ���� ���� �ʴ´�.
 *	Concept
 *										ComponentBase
					SceneComp(HasTransformData,Hierarchy)			SomeObjectCompont(No Transform Mainly Logic)
 */


class ComponentBase : public XObject
{
	CLASS(ComponentBase, XObject, Abstract)
	friend class GameObject;	

protected:
	ComponentBase();
	ComponentBase(const ComponentBase& Other);			
	virtual ~ComponentBase();


public:
	/* Component's Interface */
	virtual void BeginPlay();	

	
	virtual void PreUpdate(float DeltaSeconds)			override;

	virtual void Update(float DeltaSeconds)				override;

	virtual void PostUpdate(float DeltaSeconds)			override;

	virtual void EndPlay();

	void SetLifeTime(const float InLifeTime);
		
	virtual void Destroy(float TimeDelay = -1.0f);

	virtual void SetOwningScene(class Scene* InScene);

	class Scene* GetOwningScene() const;

	bool IsPlaying() const;

protected:
	virtual void OnDestroy()							 override;

protected:
	class Scene* m_OwningScene;

	float			m_LifeTime;

	uint64			m_DestroyTimerID;

	TimerHandle		m_TimerHandle;

	bool			m_IsBegun;

};

