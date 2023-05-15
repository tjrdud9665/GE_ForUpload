#pragma once

#include "PropertyWidget.h"

/* Transform �� ǥ���ϱ����� �뵵�θ� ��� 
* STransform �� ������� Relative �� �����ϱⰡ ��ٷӴ�(�Ұ������ƴ�) 
* ->STransform ��  Owner or conatiner �� Relative �䱸�ϸ� ������ ������ ����������.PASs
*/

class TransformPropertyWidget :public PropertyWidget
{
	CLASS(TransformPropertyWidget,PropertyWidget)

protected:
	TransformPropertyWidget();
	virtual ~TransformPropertyWidget();

	virtual void Update(float DeltaSeconds) override;

	TPropertyHandle<class SceneComponent*>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<class SceneComponent*>*>(m_Prop);
	}

private:
	bool	m_DrawLocationModePopup;
	bool	m_DrawRotationModePopup;
	bool	m_DrawScale3DModePopup;
};

