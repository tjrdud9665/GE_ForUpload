#pragma once

#include "../AssetData.h"
#include "../TextureAsset/TextureAsset.h"

class ParticleAsset : public AssetData
{
	CLASS(ParticleAsset, AssetData)
	friend class ParticleComponent;

protected:
	ParticleAsset();
	ParticleAsset(const ParticleAsset& Other);
	virtual ~ParticleAsset();	
	
	
	void MakeParticleInfo(SParticleUpdateInfo& OutParticleInfo);

public:
	void BindRenderingContext(class Renderer* pRenderer);


private:
	//TODO  Make Sprite
	PROP(TSharedPtr<class TextureAsset>, ParticleTexture, DetailsEdit, "Texture")
	TSharedPtr<class TextureAsset> ParticleTexture;


private:
	PROP(int32, SpawnCountMax, DetailsEdit,"Spawn")
	int32						SpawnCountMax;


	PROP(SVector3, StartMin, DetailsEdit, "Postion")
	SVector3	StartMin;

	PROP(SVector3, StartMax, DetailsEdit, "Postion")
	SVector3	StartMax; 


	PROP(SVector3, ScaleMin, DetailsEdit, "Scale")
	SVector3	ScaleMin;

	PROP(SVector3, ScaleMax, DetailsEdit, "Scale")
	SVector3	ScaleMax;

	PROP(float, LifeTimeMin, DetailsEdit, "LifeTime")
	float LifeTimeMin;

	PROP(float, LifeTimeMax, DetailsEdit, "LifeTime")
	float LifeTimeMax;


	PROP(SLinearColor, ColorMin, DetailsEdit, "Color")
	SLinearColor		ColorMin;

	PROP(SLinearColor, ColorMax, DetailsEdit, "Color")
	SLinearColor		ColorMax;

	PROP(SVector3, MoveDirMin, DetailsEdit, "Direction")
	SVector3	MoveDirMin;

	PROP(SVector3, MoveDirMax, DetailsEdit, "Direction")
	SVector3	MoveDirMax;

	PROP(float, SpeedMin, DetailsEdit, "Speed")
	float	SpeedMin;
	
	PROP(float, SpeedMax, DetailsEdit, "Speed")
	float	SpeedMax;

	PROP(int32, SpriteCol, DetailsEdit, "Sprite")
	int32	SpriteCol;

	PROP(int32, SpriteRow, DetailsEdit, "Sprite")
	int32	SpriteRow;

	float										m_SpawnTime;

};

