#include "EnginePCH.h"
#include "Vector2.h"

SVector2::SVector2()
	:x(0.0f)
	,y(0.0f)
{

}

SVector2::SVector2(float _Inf)
	:x(_Inf)
	,y(_Inf)
{

}

SVector2::SVector2(float _x, float _y)
	:x(_x)
	,y(_y)
{

}

SVector2::SVector2(const SVector2& v)
	:x(v.x)
	,y(v.y)
{

}

SVector2::SVector2(const XMVECTOR& v)
	:x(0.0f)
	, y(0.0f)
{
	::XMStoreFloat2((XMFLOAT2*)this, v);
}

SVector2::SVector2(int32 _x, int32 _y)
	:x(static_cast<float>(_x))
	,y(static_cast<float>(_y))
{
	
}

SVector2::SVector2(int32 _InI)
	:x(static_cast<float>(_InI))
	,y(static_cast<float>(_InI))
{

}

SVector2::SVector2(uint32 _x, uint32 _y)
	:x(static_cast<float>(_x))
	,y(static_cast<float>(_y))
{

}

SVector2::SVector2(uint32 _InI)
	:x(static_cast<float>(_InI))
	, y(static_cast<float>(_InI))
{

}

SVector2::SVector2(const struct SVector3& v)
	:x(v.x)
	,y(v.y)
{

}

SVector2& SVector2::operator=(const struct SVector3& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

float SVector2::operator^(const SVector2& V) const
{
	return x * V.y - y * V.x;
}

float SVector2::operator|(const SVector2& V) const
{
	return x * V.x + y * V.y;
}

float SVector2::Size() const
{		
	return ::sqrt(x * x + y * y);
}

float SVector2::SizeSquared() const
{
	return x * x + y * y;
}

void SVector2::Normalize(float Tolerance /*= SMALL_NUMBER*/)
{
	const float SquareSum = x * x + y * y;
	if (SquareSum > Tolerance)
	{
		const float Scale = SMathUtils::InvSqrt(SquareSum);
		x *= Scale;
		y *= Scale;
		return;
	}
	x = 0.0f;
	y = 0.0f;
}

SVector2 SVector2::GetSafeNormal(float Tolerance /*= SMALL_NUMBER*/) const
{
	const float SquareSum = x * x + y * y;
	if (SquareSum > Tolerance)
	{
		const float Scale = SMathUtils::InvSqrt(SquareSum);
		return SVector2(x * Scale, y * Scale);
	}
	return SVector2(0.f, 0.f);
}

SVector2 SVector2::GetAbs() const
{
	return SVector2(::abs(x), ::abs(y));
}

void SVector2::Abs()
{
	x = ::abs(x);
	y = ::abs(y);
}

float SVector2::DistSquared(const SVector2& V1, const SVector2& V2)
{
	return SMathUtils::Square(V2.x - V1.x) + SMathUtils::Square(V2.y - V1.y);
}

SVector2 SVector2::GetRotated(const float AngleDeg) const
{
	// Based on FVector::RotateAngleAxis with Axis(0,0,1)

	float S, C;
	SMathUtils::SinCos(&S, &C, SMathUtils::DegreesToRadians(AngleDeg));

	//const float OMC = 1.0f - C;

	return SVector2(
		C * x - S * y,
		S * x + C * y);
}

float SVector2::GetAngle(const SVector2& v) const
{
	SVector2	v1 = *this;
	SVector2	v2 = v;

	v1.Normalize();
	v2.Normalize();

	return SVector2::GetAngle(v1, v2);
	
}

float SVector2::GetAngle(const SVector2& v1, const SVector2& v2)
{
	float	Angle = SVector2::DotProduct(v1, v2);

	SMathUtils::RadiansToDegrees(::acosf(Angle));

	return Angle;
}

float SVector2::Distance(const SVector2& V1, const SVector2& V2)
{
	return ::sqrt(SVector2::DistSquared(V1, V2));
}

float SVector2::DotProduct(const SVector2& A, const SVector2& B)
{
	return A.x * B.x + A.y * B.y;
	//return A | B;
}

DirectX::XMVECTOR SVector2::Convert() const
{
	return XMLoadFloat2((XMFLOAT2*)this);
}

void SVector2::Convert(const XMVECTOR& v)
{
	XMStoreFloat2((XMFLOAT2*)this, v);
}

const SVector2 SVector2::ZeroVector = SVector2(0,0);

const SVector2 SVector2::OneVector = SVector2(1.0f, 1.0f);

const SVector2 SVector2::HalfVector = SVector2(0.5f, 0.5f);

const SVector2 SVector2::RightVector = SVector2(1, 0);

const SVector2 SVector2::LeftVector = SVector2(-1, 0);;

const SVector2 SVector2::UpVector = SVector2(0, 1);;

const SVector2 SVector2::DownVector = SVector2(0, -1);;

const SVector2& SVector2::operator--(int)
{
	x -= 1.f;
	y -= 1.f;

	return *this;
}

const SVector2& SVector2::operator--()
{
	x -= 1.f;
	y -= 1.f;

	return *this;
}

const SVector2& SVector2::operator++(int)
{
	x += 1.f;
	y += 1.f;

	return *this;
}

const SVector2& SVector2::operator++()
{
	x += 1.f;
	y += 1.f;

	return *this;

}

SVector2& SVector2::operator/=(const float f)
{
	x /= f;
	y /= f;
	return *this;
}

SVector2& SVector2::operator/=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();

	Convert(v1 / v);
	return *this;
}

SVector2& SVector2::operator/=(const SVector2& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(v1 / v2);
	return *this;
}

SVector2 SVector2::operator/(const float f) const
{
	return SVector2(x / f, y / f);
}

SVector2 SVector2::operator/(const XMVECTOR& v) const
{
	return SVector2(Convert() / v);
}

SVector2 SVector2::operator/(const SVector2& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector2(v1 / v2);
}

SVector2 SVector2::operator*=(const float f)
{
	x *= f;
	y *= f;

	return *this;
}

SVector2 SVector2::operator*=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();
	
	Convert(XMVectorMultiply(v1, v));

	return *this;
}

SVector2 SVector2::operator*=(const SVector2& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();
		
	Convert(XMVectorMultiply(v1, v2));

	return *this;
}

SVector2 SVector2::operator*(const float f) const
{
	return SVector2(x * f, y * f);
}

SVector2 SVector2::operator*(const XMVECTOR& v) const
{		
	return SVector2(XMVectorMultiply(Convert(), v));
}

SVector2 SVector2::operator*(const SVector2& v) const
{

	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();
	
	return SVector2(XMVectorMultiply(v1, v2));
}

SVector2& SVector2::operator-=(const float f)
{
	x -= f;
	y -= f;
	return *this;
}

SVector2& SVector2::operator-=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();
	
	Convert(XMVectorSubtract(v1, v));
	return *this;
}

SVector2& SVector2::operator-=(const SVector2& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorSubtract(v1, v2));
	return *this;
}

SVector2 SVector2::operator-(const float f) const
{
	return SVector2(x - f, y - f);
}

SVector2 SVector2::operator-(const XMVECTOR& v) const
{	
	return SVector2(XMVectorSubtract(Convert(), v));
}

SVector2 SVector2::operator-(const SVector2& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();
	
	return XMVectorSubtract(v1, v2);
}

SVector2& SVector2::operator+=(const float f)
{
	x += f;
	y += f;

	return *this;
}

SVector2& SVector2::operator+=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();	
	Convert(XMVectorAdd(v1 , v));
	return *this;
}

SVector2& SVector2::operator+=(const SVector2& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorAdd(v1 , v2));

	return *this;
}

SVector2 SVector2::operator+(const float f) const
{
	return SVector2(x + f, y + f);
}

SVector2 SVector2::operator+(const XMVECTOR& v) const
{
	return SVector2(XMVectorAdd(Convert(), v));
}

SVector2 SVector2::operator+(const SVector2& v) const
{
	XMVECTOR v1 = Convert();
	XMVECTOR v2 = v.Convert();

	return SVector2(XMVectorAdd(v1 , v2));
}

float& SVector2::operator[](int Index)
{
	if (Index < 0)
		return x;
	if (Index > 1)
		return y;


	return (&x)[Index];

}

float SVector2::operator[](int Index) const
{
	if (Index < 0)
		return x;
	if (Index > 1)
		return y;


	return (&x)[Index];

}

bool SVector2::operator!=(const XMVECTOR& v) const
{
	SVector2	v1;
	XMStoreFloat2((XMFLOAT2*)&v1, v);

	return x != v1.x || y != v1.y;
}

bool SVector2::operator!=(const SVector2& v) const
{
	return x != v.x || y != v.y;
}

bool SVector2::operator==(const XMVECTOR& v) const
{
	SVector2	v1;
	XMStoreFloat2((XMFLOAT2*)&v1, v);

	return x == v1.x && y == v1.y;
	
}

bool SVector2::operator==(const SVector2& v) const
{
	return x == v.x && y == v.x;
}

SVector2& SVector2::operator=(float f)
{
	x = f;
	y = f;

	return *this;
}

SVector2& SVector2::operator=(const XMVECTOR& v)
{
	XMStoreFloat2((XMFLOAT2*)this, v);

	return *this;
}

SVector2& SVector2::operator=(const SVector2& v)
{
	x = v.x;
	y = v.y;

	return *this;
}
