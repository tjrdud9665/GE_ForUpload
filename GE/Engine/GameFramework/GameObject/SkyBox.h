#pragma once
#include "GameObject.h"

class SkyBox :public GameObject
{
	CLASS(SkyBox, GameObject)

protected:
	SkyBox();
	SkyBox(const SkyBox& Other);
	virtual ~SkyBox();

	virtual void Construct(const std::string& InName /* = "" */) override;

private:
	class SkyComponent* m_SkyComponent;
};

