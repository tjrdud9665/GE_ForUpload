#include "EnginePCH.h"
#include "Vector3.h"


const SVector3 SVector3::ZeroVector = SVector3(0.0f);

const SVector3 SVector3::OneVector = SVector3(1.0f);

const SVector3 SVector3::UpVector = SVector3(0 , 1.0f, 0.0f);

const SVector3 SVector3::DownVector = SVector3(0, -1.0f , 0.0f);

const SVector3 SVector3::ForwardVector = SVector3(0.0f, 0.0f, 1.0f);

const SVector3 SVector3::BackwardVector = SVector3(0.0f, 0.0f, -1.0f);

const SVector3 SVector3::RightVector = SVector3(1.0f, 0.0f, 0.0f);

const SVector3 SVector3::LeftVector = SVector3(-1.0f, 0.0f, 0.0f);;

const SVector3 SVector3::XAxisVector = SVector3(1.0f, 0.0f, 0.0f);

const SVector3 SVector3::YAxisVector = SVector3(0.0f, 1.0f, 0.0f);

const SVector3 SVector3::ZAxisVector = SVector3(0.0f, 0.0f, 1.0f);

const SVector3 SVector3::Axis[EAxis::MAX] =
{
	XAxisVector,
	YAxisVector,
	ZAxisVector
};


SVector3::SVector3()
	:x(0.0f)
	,y(0.0f)
	,z(0.0f)
{

}

SVector3::SVector3(float _x, float _y, float _z)
	:x(_x)
	,y(_y)
	,z(_z)
{

}

SVector3::SVector3(const SVector3& v)
	:x(v.x)
	,y(v.y)
	,z(v.z)
{

}

SVector3::SVector3(const XMVECTOR& v)
	:x(0.0f)
	,y(0.0f)
	,z(0.0f)
{
	XMStoreFloat3((XMFLOAT3*)this, v);
}



SVector3::SVector3(float _f)
	:x(_f)
	,y(_f)
	,z(_f)
{

}



SVector3::SVector3(const struct SVector2& v, float _z)
	:x(v.x)
	,y(v.y)
	,z(_z)
{
	
}

SVector3::SVector3(const struct SVector2& v)
	:x(v.x)
	, y(v.y)
	,z(0.0f)
{

}

SVector3::SVector3(const struct SVector4& v)
	:x(v.x)
	, y(v.y)
	, z(v.z)
{

}



SVector3 SVector3::operator-(const float f) const
{
	return SVector3(x - f, y - f, z - f);
}

SVector3 SVector3::operator-(const XMVECTOR& v) const
{	
	return SVector3(XMVectorSubtract(Convert(), v));
}

SVector3 SVector3::operator-(const SVector3& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector3(XMVectorSubtract(v1 , v2));
}


SVector3 SVector3::operator+(const float f) const
{
	return SVector3(x + f, y + f, z + f);
}

SVector3 SVector3::operator+(const XMVECTOR& v) const
{
	return SVector3(XMVectorAdd(Convert() , v));
}

SVector3 SVector3::operator+(const SVector3& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector3(XMVectorAdd(v1 , v2));

}

SVector3& SVector3::operator+=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();
	
	Convert(XMVectorAdd(v1 , v));

	return *this;
}

SVector3& SVector3::operator+=(const SVector3& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorAdd(v1 , v2));

	return *this;


}

SVector3& SVector3::operator+=(const float f)
{
	x += f;
	y += f;
	z += f;

	return *this;
}


float& SVector3::operator[](int Index)
{
	if (Index < 0)
		return x;
	if (Index > 2)
		return z;

	return (&x)[Index];
}

float SVector3::operator[](int32 Index) const
{
	if (Index < 0)
		return x;		
	if (Index > 2)
		return z;


	return (&x)[Index];
}


bool SVector3::operator!=(const XMVECTOR& v) const
{
	SVector3	v1;
	XMStoreFloat3((XMFLOAT3*)&v1, v);

	return x != v1.x || y != v1.y || z != v1.z;
}

bool SVector3::operator!=(const SVector3& v) const
{
	return x != v.x || y != v.y || z != v.z;
}

bool SVector3::operator==(const XMVECTOR& v) const
{
	SVector3	v1;
	XMStoreFloat3((XMFLOAT3*)&v1, v);

	return x == v1.x && y == v1.y && z == v1.z;
}

bool SVector3::operator==(const SVector3& v) const
{
	return x == v.x && y == v.y && z == v.z;
}

SVector3& SVector3::operator-=(const SVector3& v)
{

	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorSubtract(v1 , v2));
	return *this;
}

SVector3& SVector3::operator-=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();
		
	Convert(XMVectorSubtract(v1 , v));
	return *this;
}

SVector3& SVector3::operator-=(const float f)
{
	x -= f;
	y -= f;
	z -= f;

	return *this;
}

SVector3 SVector3::operator*(const SVector3& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();
	
	return SVector3(XMVectorMultiply(v1 , v2));

}

SVector3 SVector3::operator*(const XMVECTOR& v) const
{
	return SVector3(XMVectorMultiply(Convert() , v));
}

SVector3 SVector3::operator*(const float f) const
{
	return SVector3(x * f, y * f, z * f);

}

SVector3& SVector3::operator*=(const SVector3& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorMultiply(v1 , v2));

	return *this;
}

SVector3& SVector3::operator*=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();

	Convert(XMVectorMultiply(v1 , v));
	return *this;
}

SVector3& SVector3::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

SVector3 SVector3::operator/(const SVector3& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector3(v1 / v2);
}

SVector3 SVector3::operator/(const XMVECTOR& v) const
{
	return SVector3(Convert() / v);
}

SVector3 SVector3::operator/(const float f) const
{
	return SVector3(x / f, y / f, z / f);
}

SVector3& SVector3::operator/=(const SVector3& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(v1 / v2);

	return *this;
}

SVector3& SVector3::operator/=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();

	Convert(v1 / v);
	return *this;
}

SVector3& SVector3::operator/=(const float f)
{
	x /= f;
	y /= f;
	z /= f;

	return *this;
}

const SVector3& SVector3::operator++()
{
	x += 1.f;
	y += 1.f;
	z += 1.f;

	return *this;
}

const SVector3& SVector3::operator++(int)
{
	
	x += 1.f;
	y += 1.f;
	z += 1.f;

	return *this;
}

const SVector3& SVector3::operator--()
{
	x -= 1.f;
	y -= 1.f;
	z -= 1.f;

	return *this;
}

const SVector3& SVector3::operator--(int)
{
	x -= 1.f;
	y -= 1.f;
	z -= 1.f;

	return *this;
}

float SVector3::DistSquared(const SVector3& V1, const SVector3& V2)
{
	return SMathUtils::Square(V2.x - V1.x) + SMathUtils::Square(V2.y - V1.y) + SMathUtils::Square(V2.z - V1.z);
}

float SVector3::Distance(const SVector3& V1, const SVector3& V2)
{
	return ::sqrt(SVector3::DistSquared(V1, V2));
}

SVector3 SVector3::operator^(const SVector3& V) const
{
	return SVector3
	(
		y * V.z - z * V.y,
		z * V.x - x * V.z,
		x * V.y - y * V.x
	);
}

SVector3 SVector3::CrossProduct(const SVector3& A, const SVector3& B)
{
	return A ^ B;
}

float SVector3::operator|(const SVector3& V) const
{
	return x * V.x + y * V.y + z * V.z;
}

float SVector3::DotProduct(const SVector3& A, const SVector3& B)
{
	return A | B;
}



float SVector3::SizeSquared() const
{
	return x * x + y * y + z* z ;
}


float SVector3::Size() const
{
	return ::sqrt(x * x + y * y + z * z);
}

SVector3 SVector3::GetSafeNormal(float Tolerance /*= SMALL_NUMBER*/) const
{
	const float SquareSum = x * x + y * y + z * z;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		return *this;
	}
	else if (SquareSum < Tolerance)
	{
		return SVector3::ZeroVector;
	}
	const float Scale = SMathUtils::InvSqrt(SquareSum);
	return SVector3(x * Scale, y * Scale, z * Scale);
}

bool SVector3::Normalize(float Tolerance /*= SMALL_NUMBER*/)
{
	const float SquareSum = x * x + y * y + z * z;
	if (SquareSum > Tolerance)
	{
		const float Scale = SMathUtils::InvSqrt(SquareSum);
		x *= Scale; y *= Scale; z *= Scale;
		return true;
	}
	return false;
}

SVector3 SVector3::GetReciprocal(float Tolerance ) const
{
	SVector3 SafeReciprocalScale;
	if (::abs(x) <= Tolerance)
	{
		SafeReciprocalScale.x = 0.f;
	} 
	else
	{
		SafeReciprocalScale.x = 1 / x;
	}

	if (::abs(y) <= Tolerance)
	{
		SafeReciprocalScale.y = 0.f;
	} 
	else
	{
		SafeReciprocalScale.y = 1 / y;
	}

	if (::abs(z) <= Tolerance)
	{
		SafeReciprocalScale.z = 0.f;
	} 
	else
	{
		SafeReciprocalScale.z = 1 / z;
	}

	return SafeReciprocalScale;
}

DirectX::XMVECTOR SVector3::Convert() const
{
	return XMLoadFloat3((XMFLOAT3*)this);
}

void SVector3::Convert(const XMVECTOR& v)
{
	XMStoreFloat3((XMFLOAT3*)this, v);
}


SVector3 SVector3::ConvertAngle() const
{
	return SVector3(SMathUtils::DegreesToRadians(x), SMathUtils::DegreesToRadians(y), SMathUtils::DegreesToRadians(z));
}

SVector3 SVector3::TransformNormal(const Matrix& m) const
{
	return SVector3(XMVector3TransformNormal(Convert(), m.m));
}

SVector3 SVector3::TransformCoord(const Matrix& m) const
{
	return SVector3(XMVector3TransformCoord(Convert(), m.m));
}

SVector3 SVector3::RotateAngleAxis(const float AngleDeg, const SVector3& Axis)
{
	float S, C;
	SMathUtils::SinCos(&S, &C, SMathUtils::DegreesToRadians(AngleDeg));

	const float XX = Axis.x * Axis.x;
	const float YY = Axis.y * Axis.y;
	const float ZZ = Axis.z * Axis.z;

	const float XY = Axis.x * Axis.y;
	const float YZ = Axis.y * Axis.z;
	const float ZX = Axis.z * Axis.x;

	const float XS = Axis.x * S;
	const float YS = Axis.y * S;
	const float ZS = Axis.z * S;

	const float OMC = 1.f - C;

	return SVector3(
		(OMC * XX + C) * x + (OMC * XY - ZS) * y + (OMC * ZX + YS) * z,
		(OMC * XY + ZS) * x + (OMC * YY + C) * y + (OMC * YZ - XS) * z,
		(OMC * ZX - YS) * x + (OMC * YZ + XS) * y + (OMC * ZZ + C) * z
	);
}

SVector3 SVector3::VInterpTo(const SVector3& Current, const SVector3& Target, float DeltaTime, float InterpSpeed)
{
	// If no interp speed, jump to target value
	if (InterpSpeed <= 0.f)
	{
		return Target;
	}

	// Distance to reach
	const SVector3 Dist = Target - Current;

	// If distance is too small, just set the desired location
	if (Dist.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return Target;
	}

	// Delta Move, Clamp so we do not over shoot.
	const SVector3	DeltaMove = Dist * SMathUtils::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return Current + DeltaMove;
}

SVector3& SVector3::operator=(float f)
{
	x = y = z = f;

	return *this;
}

SVector3& SVector3::operator=(const XMVECTOR& v)
{
	XMStoreFloat3((XMFLOAT3*)this, v);

	return *this;
}

SVector3& SVector3::operator=(const SVector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;

}


