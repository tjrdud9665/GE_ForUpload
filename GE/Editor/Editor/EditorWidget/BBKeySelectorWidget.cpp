#include "PCH.h"
#include "BBKeySelectorWidget.h"
#include "imgui.h"
#include <Assets/AI/BehaviorTree.h>
#include <Assets/AI/BlackBoard.h>

BBKeySelectorWidget::BBKeySelectorWidget()
{

}

BBKeySelectorWidget::~BBKeySelectorWidget()
{

}

void BBKeySelectorWidget::PropertyGUI()
{
	std::string name = "##" + GetName();
	
	TPropertyHandle<SBlackboardKeySelector>* Handle = GetHandle();
	SBlackboardKeySelector* bbSelector = Handle->GetValuePtr();

	if (!bbSelector)
		return;

	BTNode* owner = (BTNode*)Handle->GetInstance();
	
	if(ImGui::BeginCombo(name.c_str(), bbSelector->SeletedKeyName.empty() ? "None" : bbSelector->SeletedKeyName.c_str()))
	{
		Blackboard* bb = owner->GetBTree()->GetBlackBoard();		

		for(int32 i = 0;i < (int32)EBlackBoardValType::MAX;i++)
		{
			EBlackBoardValType allowType = EBlackBoardValType(i);
			std::vector<EBlackBoardValType>::const_iterator found = std::find_if(bbSelector->AllowedTypes.begin(), bbSelector->AllowedTypes.end(), [allowType](const EBlackBoardValType& valType)
			{
				return valType == allowType;
			});

			if(found != bbSelector->AllowedTypes.end())
			{
				std::vector<std::string> keys;
				size_t keySize =  bb->GetValueKeys(allowType, OUT keys);				

				for(size_t k=0;k<keySize;k++)
				{
					bool IsSelected = bbSelector->SeletedKeyName == keys[k];
					if(ImGui::Selectable(keys[k].c_str(),IsSelected))
					{
						bbSelector->SeletedKeyName = keys[k];
						bbSelector->SeletedKeyType = allowType;
					}
				}				
			}
		}

		ImGui::EndCombo();
	}

	
}
