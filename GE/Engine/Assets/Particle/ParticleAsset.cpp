#include "EnginePCH.h"
#include "ParticleAsset.h"

ParticleAsset::ParticleAsset()
	: SpawnCountMax(100)
	, StartMin(SVector3(-10.0f))
	, StartMax(SVector3(10.0f))
	, ScaleMin(SVector3(-30.0f))
	, ScaleMax(SVector3(30.0f))
	, LifeTimeMin(0.5f)
	, LifeTimeMax(1.0f)
	, ColorMin(SLinearColor(1.0f, 1.0f, 1.0f))
	, ColorMax(SLinearColor(1.0f, 1.0f, 1.0f))
	, MoveDirMin(SVector3(-1.0f))
	, MoveDirMax(SVector3(1.0f))
	, SpeedMin(0.0f)
	, SpeedMax(0.0f)
	, SpriteCol(1)
	, SpriteRow(1)
{

}

ParticleAsset::ParticleAsset(const ParticleAsset& Other)
	:Super(Other)
	, ParticleTexture(Other.ParticleTexture)
	, SpawnCountMax(Other.SpawnCountMax)
	, StartMin(Other.StartMin)
	, StartMax(Other.StartMax)
	, ScaleMin(Other.ScaleMin)
	, ScaleMax(Other.ScaleMax)
	, LifeTimeMin(Other.LifeTimeMin)
	, LifeTimeMax(Other.LifeTimeMax)
	, ColorMin(Other.ColorMin)
	, ColorMax(Other.ColorMax)
	, MoveDirMin(Other.MoveDirMin)
	, MoveDirMax(Other.MoveDirMax)
	, SpeedMin(Other.SpeedMin)
	, SpeedMax(Other.SpeedMax)
	, SpriteCol(Other.SpriteCol)
	, SpriteRow(Other.SpriteRow)
{

}

ParticleAsset::~ParticleAsset()
{

}

void ParticleAsset::MakeParticleInfo(SParticleUpdateInfo& OutParticleInfo)
{
	OutParticleInfo.ColorMax = ColorMax.ToVector4();
	OutParticleInfo.ColorMin = ColorMin.ToVector4();

	OutParticleInfo.LifeTimeMax = LifeTimeMax;
	OutParticleInfo.LifeTimeMin = LifeTimeMin;

	OutParticleInfo.MoveDirMax = MoveDirMax;
	OutParticleInfo.MoveDirMin = MoveDirMin;

	OutParticleInfo.ScaleMax = ScaleMax;
	OutParticleInfo.ScaleMin = ScaleMin;

	OutParticleInfo.SpawnCountMax = SpawnCountMax;

	OutParticleInfo.StartMin = StartMin;
	OutParticleInfo.StartMax = StartMax;

	OutParticleInfo.SpeedMax = SpeedMax;
	OutParticleInfo.SpeedMin = SpeedMin;

	OutParticleInfo.SpriteCol = SpriteCol;
	OutParticleInfo.SpriteRow = SpriteRow;
	OutParticleInfo.SpriteCount = ::max(1,SpriteRow*SpriteCol);

}

void ParticleAsset::BindRenderingContext(class Renderer* pRenderer)
{
	float TexOn = 0.0f;	
	if(ParticleTexture)
	{
		TexOn = 1.0f;
		pRenderer->SetTexture("texDiffuseMap",ParticleTexture->GetTextureID());
		pRenderer->SetSamplerState("sLinearSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
	}
	pRenderer->SetConstant1f("hasTex", TexOn);
}

