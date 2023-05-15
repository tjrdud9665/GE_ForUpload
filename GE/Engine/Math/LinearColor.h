#pragma once

struct SLinearColor
{
	float r;
	float g;
	float b;
	float a;
	
	SLinearColor()
		:r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{
	}
	SLinearColor(float _r,float _g,float _b, float _a = 1.0f)
		:r(_r),g(_g),b(_b),a(_a)
	{

	}

	SLinearColor(const SLinearColor& Other)
		:r(Other.r)
		, g(Other.g)
		, b(Other.b)
		, a(Other.a)
	{
		
	}

	SLinearColor(const struct SVector4& FromVector);

	SLinearColor(const struct SVector3& FromVector);

	struct SVector3 ToVector3() const;
	struct SVector4 ToVector4() const;

	uint32 ToUINT() const;


	// Operators.
	bool operator==(const SLinearColor& C) const;

	bool operator!=(const SLinearColor& C) const;

	/** Some pre-inited colors, useful for debug code */
	static  const SLinearColor White;
	static  const SLinearColor Gray;
	static  const SLinearColor Black;
	static  const SLinearColor Transparent;
	static  const SLinearColor Red;
	static  const SLinearColor Green;
	static  const SLinearColor Blue;
	static  const SLinearColor Yellow;

};

