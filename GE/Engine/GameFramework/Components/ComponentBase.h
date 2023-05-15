#pragma once

#include "../../XObject/XObject.h"
#include "../../Timer/TimerHandle.h"

/**
 *  Component 의 기반 클래스입니다. 
 *  학원과정에 맞게 컴포넌트는 렌더링과 계층구조를 가지는 sceneComponent 와 로직을 가지는 ObjectComponet로 나눕니다.
 *  Component 는 개별 에셋으로 저장 하지 않는다.
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

