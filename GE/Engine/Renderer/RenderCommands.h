#pragma once

#include "Shader.h"

class Renderer;

constexpr size_t TEXTURE_ARRAY_SIZE = 32;
struct SSetTextureCommand
{
	SSetTextureCommand(FTextureID texID, const STextureBinding& shaderBinding, uint32 sliceIn, bool bUA = false)
		: TextureIDs({ texID })
		, Binding(shaderBinding)
		, bUnorderedAccess(bUA)
		, slice(sliceIn)
		, numTextures(1)
	{}
	//Array
	SSetTextureCommand(const std::array<FTextureID, TEXTURE_ARRAY_SIZE>& texIDs, uint32 numTexturesIn, const STextureBinding& shaderBinding, uint32 sliceIn, bool bUA = false)
		: TextureIDs(texIDs)
		, Binding(shaderBinding)
		, bUnorderedAccess(bUA)
		, slice(sliceIn)
		, numTextures(numTexturesIn)
	{}

	void SetResource(Renderer* pRenderer);

	const std::array<FTextureID, TEXTURE_ARRAY_SIZE> TextureIDs;
	const STextureBinding&  Binding;
	const uint32			slice;
	const uint32			numTextures;
	bool bUnorderedAccess;
};

struct SSetSamplerCommand
{
	SSetSamplerCommand(FSamplerID sampID, const SSamplerBinding& shaderSamp) 
		: SamplerID(sampID), Binding(shaderSamp) 
	{
	}
	void SetResource(Renderer* pRenderer);	

	const FSamplerID			SamplerID;
	const SSamplerBinding&		Binding;
};

struct SSetRWBufferCommand
{
	SSetRWBufferCommand(FBufferID UAVBufferID ,  const SRWBinding& bindable)
		:BufferID(UAVBufferID)
		,Binding(bindable)
	{		
	}

	void SetResource(class Renderer* pRenderer);

	const FBufferID		BufferID;
	const SRWBinding	Binding;
};


struct SClearCommand
{
	static SClearCommand Depth(float depthClearValue);
	static SClearCommand Color(const SVector4& InClearColor);
	static SClearCommand Color(float r, float g, float b, float a);

	SClearCommand(bool doClearColor, bool doClearDepth, bool doClearStencil,
				 const SVector4& clearColorValues, float clearDepthValue, uchar8 clearStencilValue) :
		bDoClearColor(doClearColor),
		bDoClearDepth(doClearDepth),
		bDoClearStencil(doClearStencil),
		ClearColor(clearColorValues),
		ClearDepth(clearDepthValue),
		ClearStencil(clearStencilValue)
	{

	}


	bool					bDoClearColor;
	bool					bDoClearDepth;
	bool					bDoClearStencil;

	SVector4				ClearColor;
	float					ClearDepth;
	uchar8					ClearStencil;
};

struct SDrawLineCommand
{
	SVector3 p1;
	SVector3 p2;

	SLinearColor color;

	SDrawLineCommand(const SVector3& _p1 , const SVector3& _p2, const SLinearColor& InColor)
		:p1(_p1),p2(_p2),color(InColor)
	{
	}	

	void SetConstants(std::vector<SlineInfo>& lines,int32 idx);
};

struct SDrawSolidCommand
{
	SVector3 p1;
	SVector3 p2;
	SVector3 p3;

	SLinearColor color;

	SDrawSolidCommand(const SVector3& _p1, const SVector3& _p2, const SVector3& _p3, const SLinearColor& InColor)
		:p1(_p1), p2(_p2), p3(_p3) ,color(InColor)
	{}

	void SetConstants(std::vector<SSolidInfo>& solids, int32 idx);
};