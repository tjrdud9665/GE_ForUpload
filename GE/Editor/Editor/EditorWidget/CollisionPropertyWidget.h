#pragma once
#include "PropertyWidget.h"


//Array 로 Profile 을 Editor 에 서 노출 X

class CollisionPropertyWidget :public PropertyWidget
{
	CLASS(CollisionPropertyWidget, PropertyWidget)

public:
	CollisionPropertyWidget();
	virtual ~CollisionPropertyWidget();


	virtual void Update(float DeltaSeconds) override;

private:
	void ShowCollisionChannelCombo(SCollisionProfile** Profile);

	void ShowCollisionProfilesCombo(SCollisionProfile** Profile);


	//idx == INDEX_NONE인경우 전체를 컨트롤하는checkbox
	void ShowCollisionChannelIgnoreCheck(SCollisionProfile** Profile, int32 idx);

	void ShowCollisionChannelOverlapCheck(SCollisionProfile** Profile, int32 idx);

	void ShowCollisionChannelBlockCheck(SCollisionProfile** Profile, int32 idx);


protected:
	TPropertyHandle<SCollisionProfile*>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SCollisionProfile*>*>(m_Prop);
	}
};

