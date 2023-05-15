#include "EnginePCH.h"
#include "Vector4.h"

SVector4::SVector4()
	:x(0.0f)
	,y(0.0f)
	,z(0.0f)
	,w(0.0f)
{

}

SVector4::SVector4(float _f)
	:x(_f)
	,y(_f)
	,z(_f)
	,w(_f)

{

}

SVector4::SVector4(float _x, float _y, float _z, float _w)
	:x(_x)
	,y(_y)
	,z(_z)
	,w(_w)
{

}

SVector4::SVector4(const SVector4& v)
	:x(v.x)
	,y(v.y)
	,z(v.z)
	,w(v.w)
{

}

SVector4::SVector4(const XMVECTOR& v)
	:SVector4(0.f)
{
	XMStoreFloat4((XMFLOAT4*)this, v);
}

SVector4::SVector4(const SVector3& v, float _w)
{
	x = v.x;
	y = v.y;
	z = v.z;

	w = _w;

}

SVector4::SVector4(const struct SVector3& v)
	:x(v.x)
	,y(v.y)
	,z(v.z)
	,w(1.0f)
{

}

const float* SVector4::Data() const
{
	return &x;
}

const SVector4& SVector4::operator--(int)
{
	x -= 1.f;
	y -= 1.f;
	z -= 1.f;
	w -= 1.f;

	return *this;
}

const SVector4& SVector4::operator--()
{
	x -= 1.f;
	y -= 1.f;
	z -= 1.f;
	w -= 1.f;

	return *this;
}

const SVector4& SVector4::operator++(int)
{
	x += 1.f;
	y += 1.f;
	z += 1.f;
	w += 1.f;

	return *this;
}

const SVector4& SVector4::operator++()
{
	x += 1.f;
	y += 1.f;
	z += 1.f;
	w += 1.f;

	return *this;
}

SVector4& SVector4::operator/=(const float f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;

	return *this;
}

SVector4& SVector4::operator/=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();

	Convert(v1 / v);

	return *this;
}

SVector4& SVector4::operator/=(const SVector4& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(v1 / v2);

	return *this;
}

SVector4 SVector4::operator/(const float f) const
{
	return SVector4(x / f, y / f, z / f, w / f);
}

SVector4 SVector4::operator/(const XMVECTOR& v) const
{
	return SVector4(Convert() / v);
}

SVector4 SVector4::operator/(const SVector4& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector4(v1 / v2);
}

SVector4& SVector4::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;

	return *this;
}

SVector4& SVector4::operator*=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();

	Convert(XMVectorMultiply(v1 , v));

	return *this;
}

SVector4& SVector4::operator*=(const SVector4& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorMultiply(v1 , v2));

	return *this;
}

SVector4 SVector4::operator*(const float f) const
{
	return SVector4(x * f, y * f, z * f, w * f);
}

SVector4 SVector4::operator*(const XMVECTOR& v) const
{
	return SVector4(XMVectorMultiply(Convert() , v));
}


SVector4 SVector4::operator*(const SVector4& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector4(XMVectorMultiply(v1 , v2));
}



SVector4& SVector4::operator-=(const float f)
{
	x -= f;
	y -= f;
	z -= f;
	w -= f;

	return *this;
}

SVector4& SVector4::operator-=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();

	Convert(XMVectorSubtract(v1 , v));

	return *this;
}

SVector4& SVector4::operator-=(const SVector4& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();
	
	Convert(XMVectorSubtract(v1 , v2));

	return *this;
}

SVector4 SVector4::operator-(const float f) const
{
	return SVector4(x - f, y - f, z - f, w- f);
}

SVector4 SVector4::operator-(const XMVECTOR& v) const
{
	return SVector4(XMVectorSubtract(Convert() , v));
}

SVector4 SVector4::operator-(const SVector4& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector4(XMVectorSubtract(v1 , v2));

}

SVector4& SVector4::operator+=(const float f)
{
	x = f;
	y = f;
	z = f;
	w = f;

	return *this;
}

SVector4& SVector4::operator+=(const XMVECTOR& v)
{
	XMVECTOR	v1 = Convert();
	
	Convert(XMVectorAdd(v1 , v));

	return *this;
}

SVector4& SVector4::operator+=(const SVector4& v)
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	Convert(XMVectorAdd(v1 , v2));

	return *this;

}

SVector4 SVector4::operator+(const float f) const
{
	return SVector4(x + f, y + f, z + f, w + f);
}

SVector4 SVector4::operator+(const XMVECTOR& v) const
{
	return SVector4(XMVectorAdd(Convert() , v));
}

SVector4 SVector4::operator+(const SVector4& v) const
{
	XMVECTOR	v1 = Convert();
	XMVECTOR	v2 = v.Convert();

	return SVector4(XMVectorAdd(v1 , v2));
}

float& SVector4::operator[](int Index)
{
	if (Index < 0)
		return x;
	if (Index > 3)
		return w;

	return (&x)[Index];

}

bool SVector4::operator!=(const XMVECTOR& v) const
{
	SVector4	v1;
	XMStoreFloat4((XMFLOAT4*)&v1, v);

	return x != v1.x || y != v1.y || z != v1.z || w != v1.w;
}

bool SVector4::operator!=(const SVector4& v) const
{
	return x != v.x || y != v.y || z != v.z || w != v.w;
}

bool SVector4::operator==(const XMVECTOR& v) const
{
	SVector4	v1;
	XMStoreFloat4((XMFLOAT4*)&v1, v);

	return x == v1.x && y == v1.y && z == v1.z && w == v1.w;
}

bool SVector4::operator==(const SVector4& v) const
{
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

SVector4& SVector4::operator=(float f)
{
	x = f;
	y = f;
	z = f;
	w = f;

	return *this;
}

SVector4& SVector4::operator=(const XMVECTOR& v)
{
	XMStoreFloat4((XMFLOAT4*)this, v);
	return *this;
}

SVector4& SVector4::operator=(const SVector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;

	return *this;
}

DirectX::XMVECTOR SVector4::Convert() const
{
	return XMLoadFloat4((XMFLOAT4*)this);
}

void SVector4::Convert(const XMVECTOR& v)
{
	XMStoreFloat4((XMFLOAT4*)this, v);
}


SVector4 SVector4::Inverse() const
{
	return SVector4(-x, -y, -z, w);
}



bool SVector4::HasNan() const
{
	return isnan(x) || isnan(y) || isnan(z) || isnan(w);
}

SVector4 SVector4::VectorQuaternionMultiply2(const SVector4& QA, const SVector4& QB)
{
	const float x = QA.x;
	const float y = QA.y;
	const float z = QA.z;
	const float w = QA.w;
	const float num4 = QB.x;
	const float num3 = QB.y;
	const float num2 = QB.z;
	const float num = QB.w;
	const float num12 = (y * num2) - (z * num3);
	const float num11 = (z * num4) - (x * num2);
	const float num10 = (x * num3) - (y * num4);
	const float num9 = ((x * num4) + (y * num3)) + (z * num2);

	return SVector4(
		((x * num) + (num4 * w)) + num12,
		((y * num) + (num3 * w)) + num11,
		((z * num) + (num2 * w)) + num10,
		(w * num) - num9
	);
}

SVector4 SVector4::VectorQuaternionRotateVector(const SVector4& QA, const SVector3& VectorW0)
{
	const SVector4 QW = SVector4(QA.w);
	SVector3 T = SVector3::CrossProduct(QA, VectorW0);
	T *= 2.0f;

	const SVector4 VTemp0 = (QW * T) + VectorW0;
	const SVector4 VTemp1 = SVector3::CrossProduct(QA, T);
	const SVector4 Rotated = VTemp0 + VTemp1;
	return Rotated;
}

struct SVector3 SVector4::ToEulerAngles() const
{
	const float RAD_TO_DEG = (180.f) / PI;
	const float check = 2.0f * (-y * z + w * x);

	if (check < -0.995f)
	{
		return SVector3
		(
			-90.0f,
			0.0f,
			-atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG
		);
	}

	if (check > 0.995f)
	{
		return SVector3
		(
			90.0f,
			0.0f,
			atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG
		);
	}

	return SVector3
	(
		asinf(check) * RAD_TO_DEG,
		atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y)) * RAD_TO_DEG,
		atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)) * RAD_TO_DEG
	);


	
	//const float SingularityTest = 1.0f * (-y * z + w * x);
	//const float YawY = 2.0f * (x * z + w * y);
	//const float YawX = (1.f - 2.f * (SMathUtils::Square(x) + SMathUtils::Square(y)));


	//const float SINGULARITY_THRESHOLD = 0.4999995f;
	//const float RAD_TO_DEG = (180.f) / PI;
	//SVector3 RotatorFromQuat;

	//if (SingularityTest < -SINGULARITY_THRESHOLD)
	//{
	//	RotatorFromQuat.x = -90.f;
	//	RotatorFromQuat.y = atan2(YawY, YawX) * RAD_TO_DEG;
	//	RotatorFromQuat.z = (-RotatorFromQuat.y - (2.f * atan2(x, w) * RAD_TO_DEG));
	//} else if (SingularityTest > SINGULARITY_THRESHOLD)
	//{
	//	RotatorFromQuat.x = 90.f;
	//	RotatorFromQuat.y = atan2(YawY, YawX) * RAD_TO_DEG;
	//	RotatorFromQuat.z = (RotatorFromQuat.y - (2.f * atan2(x, w) * RAD_TO_DEG));
	//} else
	//{
	//	RotatorFromQuat.x = SMathUtils::FastAsin(2.f * (SingularityTest)) * RAD_TO_DEG;
	//	RotatorFromQuat.y = atan2(YawY, YawX) * RAD_TO_DEG;
	//	RotatorFromQuat.z = atan2(2.f * (x * y + w * z), (1.f - 2.f * (SMathUtils::Square(x) + SMathUtils::Square(z)))) * RAD_TO_DEG;
	//}

	//return RotatorFromQuat;
}

struct SVector3 SVector4::Rotate(const SVector3& v) const
{
	const SVector3 qvec = SVector3(x, y, z);
	SVector3 uv = qvec ^ v;		
	SVector3 uuv = qvec ^ uv;
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

struct SVector4 SVector4::QuaternionConjugated() const
{
	return SVector4(x, y, z, -w);
}

SVector4 SVector4::GetSafeNormal(float Tolerance /*= SMALL_NUMBER*/) const
{
	const float SquareSum = x * x + y * y + z * z + w*w;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		return *this;
	} else if (SquareSum < Tolerance)
	{
		return SVector4::ZeroVector;
	}
	const float Scale = SMathUtils::InvSqrt(SquareSum);
	return SVector4(x * Scale, y * Scale, z * Scale , w * Scale);
}

bool SVector4::Normalize(float Tolerance /*= SMALL_NUMBER*/)
{
	const float SquareSum = x * x + y * y + z * z + w * w;
	if (SquareSum > Tolerance)
	{
		const float Scale = SMathUtils::InvSqrt(SquareSum);
		x *= Scale; y *= Scale; z *= Scale; w *= Scale;
		return true;
	}
	return false;
}

const SVector4 SVector4::ZeroVector = SVector4(0,0,0,0);

const SVector4 SVector4::Black = SVector4(0.f, 0.f, 0.f, 1.f);

const SVector4 SVector4::White = SVector4(1.0f);

const SVector4 SVector4::Red = SVector4(1.f, 0.f, 0.f, 1.f);

const SVector4 SVector4::Green = SVector4(0.f, 1.f, 0.f, 1.f);

const SVector4 SVector4::Blue = SVector4(0.f, 0.f, 1.f, 1.f);

struct SVector3 operator*(const SVector3& lhs, const SVector4& rhs)
{
	return rhs * lhs;
}
