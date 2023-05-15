#include "EnginePCH.h"
#include "LinearColor.h"

SLinearColor::SLinearColor(const struct SVector4& FromVector)
	:r(FromVector.x)
	, g(FromVector.y)
	, b(FromVector.z)
	, a(FromVector.w)
{

}

SLinearColor::SLinearColor(const struct SVector3& FromVector)
	:r(FromVector.x)
	, g(FromVector.y)
	, b(FromVector.z)
	, a(1.0f)
{

}

struct SVector3 SLinearColor::ToVector3() const
{
	return SVector3(r, g, b);
}

struct SVector4 SLinearColor::ToVector4() const
{
	return SVector4(r, g, b, a);
}


uint32 SLinearColor::ToUINT() const
{
	uchar8 _r = (uchar8)(r * 255.0f);
	uchar8 _g = (uchar8)(g * 255.0f);
	uchar8 _b = (uchar8)(b * 255.0f);
	uchar8 _a = (uchar8)(a * 255.0f);
	return ((uint32)_r) | ((uint32)_g << 8) | ((uint32)_b << 16) | ((uint32)_a << 24);
}

bool SLinearColor::operator==(const SLinearColor& C) const
{
	return SMathUtils::Equals(C.r, r) && SMathUtils::Equals(C.g, g) && SMathUtils::Equals(C.b, b) && SMathUtils::Equals(C.a, a);
}



bool SLinearColor::operator!=(const SLinearColor& C) const
{
	return !SMathUtils::Equals(C.r, r) || !SMathUtils::Equals(C.g, g) || !SMathUtils::Equals(C.b, b) || !SMathUtils::Equals(C.a, a);
}

const SLinearColor SLinearColor::White(1.f, 1.f, 1.f);
const SLinearColor SLinearColor::Gray(0.5f, 0.5f, 0.5f);
const SLinearColor SLinearColor::Black(0, 0, 0);
const SLinearColor SLinearColor::Transparent(0, 0, 0, 0);
const SLinearColor SLinearColor::Red(1.f, 0, 0);
const SLinearColor SLinearColor::Green(0, 1.f, 0);
const SLinearColor SLinearColor::Blue(0, 0, 1.f);
const SLinearColor SLinearColor::Yellow(1.f, 1.f, 0);
