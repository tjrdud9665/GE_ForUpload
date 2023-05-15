#include "EnginePCH.h"
#include "XSubClass.h"
#include "XClass.h"
#include <Assets/AssetData.h>
//#include "../Assets/AssetData.h"


void XSubclassOf::operator=(class XObject* FromCustomCDO)
{
	Class = FromCustomCDO->GetClass();
	
	CustomCDOName = FromCustomCDO->GetName();


	AssetData* cdoAsset = (AssetData*)FromCustomCDO;
	if (cdoAsset)
	{
		std::string relPathWithName;
		cdoAsset->GetRelativePathWithName(OUT relPathWithName);
		CustomCDOName = relPathWithName;
	}

	

}