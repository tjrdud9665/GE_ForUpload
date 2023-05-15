#pragma once
#include "PropertyWidget.h"


//Array �� Profile �� Editor �� �� ���� X

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


	//idx == INDEX_NONE�ΰ�� ��ü�� ��Ʈ���ϴ�checkbox
	void ShowCollisionChannelIgnoreCheck(SCollisionProfile** Profile, int32 idx);

	void ShowCollisionChannelOverlapCheck(SCollisionProfile** Profile, int32 idx);

	void ShowCollisionChannelBlockCheck(SCollisionProfile** Profile, int32 idx);


protected:
	TPropertyHandle<SCollisionProfile*>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SCollisionProfile*>*>(m_Prop);
	}
};

