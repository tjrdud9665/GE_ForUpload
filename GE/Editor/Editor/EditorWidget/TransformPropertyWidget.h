#pragma once

#include "PropertyWidget.h"

/* Transform 을 표현하기위한 용도로만 사용 
* STransform 만 가지고는 Relative 에 대응하기가 까다롭다(불가능은아님) 
* ->STransform 의  Owner or conatiner 에 Relative 요구하면 가능은 하지만 불편함으로.PASs
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

