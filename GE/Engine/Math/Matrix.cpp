#include "EnginePCH.h"
#include "Matrix.h"


Matrix::Matrix()
{
	m = XMMatrixIdentity();
}


Matrix::Matrix(const Matrix& _m)
{
	m = _m.m;
}

Matrix::Matrix(const XMMATRIX& _m)
{
	m = _m;

}

Matrix::Matrix(SVector4 _v[4])
{
	::memcpy(v, _v, sizeof(SVector4) * 4);
}

void Matrix::Identity()
{
	m = XMMatrixIdentity();
}

void Matrix::Transpose()
{
	m = XMMatrixTranspose(m);
}

void Matrix::Inverse()
{
	XMVECTOR	Det = XMMatrixDeterminant(m);
	m = XMMatrixInverse(&Det, m);
}



bool Matrix::hasNAN() const
{
	return v[0].HasNan() || v[1].HasNan() || v[2].HasNan() || v[3].HasNan();
}

void Matrix::Scaling(const  SVector3& _v)
{
	m = XMMatrixScaling(_v.x, _v.y, _v.z);
}

void Matrix::Scaling(float x, float y, float z)
{
	m = XMMatrixScaling(x,y,z);
}

void Matrix::Scaling(const SVector2& _v)
{
	m = XMMatrixScaling(_v.x, _v.y, 1.f);
}

void Matrix::Scaling(float x, float y)
{
	m = XMMatrixScaling(x, y, 1.f);

}

void Matrix::Rotation(const SVector3& _v)
{
	SVector3	ConvertRot = _v.ConvertAngle();

	XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

	RotationQuaternion(Qut);
}

void Matrix::Rotation(float x, float y, float z)
{
	SVector3	_v(x, y, z);
	SVector3	ConvertRot = _v.ConvertAngle();

	XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

	RotationQuaternion(Qut);
}

void Matrix::RotationX(float x)
{
	m = XMMatrixRotationX(SMathUtils::DegreesToRadians(x));
}

void Matrix::RotationY(float y)
{
	m = XMMatrixRotationY(SMathUtils::DegreesToRadians(y));
}

void Matrix::RotationZ(float z)
{
	m = XMMatrixRotationZ(SMathUtils::DegreesToRadians(z));
}

void Matrix::RotationQuaternion(const SVector4& qut)
{
	m = XMMatrixRotationQuaternion(qut.Convert());
}

void Matrix::RotationAxis(const SVector3& Axis, float Angle)
{
	m = XMMatrixRotationAxis(Axis.Convert(), SMathUtils::DegreesToRadians(Angle));
}

void Matrix::Translation(const SVector3& _v)
{
	m = XMMatrixTranslation(_v.x, _v.y, _v.z);
}

void Matrix::Translation(float x, float y, float z)
{
	m = XMMatrixTranslation(x, y, z);
}

void Matrix::Translation(const SVector2& _v)
{
	m = XMMatrixTranslation(_v.x, _v.y, 0.f);
}

void Matrix::Translation(float x, float y)
{
	m = XMMatrixTranslation(x, y, 0.f);

}

void Matrix::DecomposeMatrix(SVector3& OutScale, SVector4& OutQuat, SVector3& OutLoc) const
{
	XMVECTOR scale;
	XMVECTOR quat;
	XMVECTOR loc;
	XMMatrixDecompose(&scale, &quat, &loc,m);

	//roll pich Yaw 

	//XMMatrixRotationRollPitchYaw(pich,yaw,roll)

	OutScale = scale;
	OutQuat = quat;
	OutLoc = loc;
}

struct SVector3 Matrix::GetScaledAxis(EAxis Axis) const
{
	switch (Axis)
	{
	case EAxis::X:
		return SVector3(m00, m10, m20);

	case EAxis::Y:
		return SVector3(m01, m11, m21);

	case EAxis::Z:
		return SVector3(m02, m12, m22);

	default:
		return SVector3::ZeroVector;
	}
}

struct SVector3 Matrix::GetUnitAxis(EAxis InAxis) const
{
	return GetScaledAxis(InAxis).GetSafeNormal();
}

SVector3 Matrix::ExtractScaling(float Tolerance /*= SMALL_NUMBER*/)
{
	SVector3 Scale3D(0, 0, 0);

	/**
	 * 	struct
	{
		float m00, m10 , m20 , m30 ;
		float m01, m11 , m21 , m31 ;
		float m02, m12 , m22 , m32 ;

		float m03, m13 , m23 , m33 ;
	};
	 */

	
	const float SquareSum0 = (m00 * m00) + (m10 * m10) + (m20 * m20);
	const float SquareSum1 = (m01 * m01) + (m11 * m11) + (m21 * m21);
	const float SquareSum2 = (m02 * m02) + (m12 * m12) + (m22 * m22);

	if (SquareSum0 > Tolerance)
	{
		float Scale0 = ::sqrt(SquareSum0);
		Scale3D[0] = Scale0;
		float InvScale0 = 1.f / Scale0;
		m00 *= InvScale0;
		m10 *= InvScale0;
		m20 *= InvScale0;
	} 
	else
	{
		Scale3D[0] = 0;
	}

	if (SquareSum1 > Tolerance)
	{
		float Scale1 = ::sqrt(SquareSum1);
		Scale3D[1] = Scale1;
		float InvScale1 = 1.f / Scale1;
		m01 *= InvScale1;
		m11 *= InvScale1;
		m21 *= InvScale1;
	} else
	{
		Scale3D[1] = 0;
	}

	if (SquareSum2 > Tolerance)
	{
		float Scale2 = ::sqrt(SquareSum2);
		Scale3D[2] = Scale2;
		float InvScale2 = 1.f / Scale2;
		m02 *= InvScale2;
		m12 *= InvScale2;
		m22 *= InvScale2;
	} else
	{
		Scale3D[2] = 0;
	}

	return Scale3D;
}

Matrix Matrix::StaticIdentity()
{
	return XMMatrixIdentity();
}

Matrix Matrix::StaticTranspose(const Matrix& _m)
{
	return XMMatrixTranspose(_m.m);
}

Matrix Matrix::StaticInverse(const Matrix& _m)
{
	XMVECTOR	Det = XMMatrixDeterminant(_m.m);
	return XMMatrixInverse(&Det, _m.m);
}

Matrix Matrix::StaticScaling(const SVector3& _v)
{
	return XMMatrixScaling(_v.x, _v.y, _v.z);
}

Matrix Matrix::StaticScaling(float x, float y, float z)
{
	return XMMatrixScaling(x, y, z);
}

Matrix Matrix::StaticScaling(const SVector2& _v)
{
	return XMMatrixScaling(_v.x, _v.y, 1.0f);
}

Matrix Matrix::StaticScaling(float x, float y)
{
	return XMMatrixScaling(x, y, 1.0f);
}

Matrix Matrix::StaticRotation(const SVector3& _v)
{
	XMMATRIX	X = XMMatrixRotationX(SMathUtils::DegreesToRadians(_v.x));
	XMMATRIX	Y = XMMatrixRotationY(SMathUtils::DegreesToRadians(_v.y));
	XMMATRIX	Z = XMMatrixRotationZ(SMathUtils::DegreesToRadians(_v.z));

	return X * Y * Z;
}

Matrix Matrix::StaticRotation(float x, float y, float z)
{
	XMMATRIX	X = XMMatrixRotationX(SMathUtils::DegreesToRadians(x));
	XMMATRIX	Y = XMMatrixRotationY(SMathUtils::DegreesToRadians(y));
	XMMATRIX	Z = XMMatrixRotationZ(SMathUtils::DegreesToRadians(z));

	return X * Y * Z;
}

Matrix Matrix::StaticRotationX(float x)
{
	return XMMatrixRotationX(SMathUtils::DegreesToRadians(x));
}

Matrix Matrix::StaticRotationY(float y)
{
	return XMMatrixRotationY(SMathUtils::DegreesToRadians(y));
}

Matrix Matrix::StaticRotationZ(float z)
{
	return XMMatrixRotationZ(SMathUtils::DegreesToRadians(z));
}

Matrix Matrix::StaticRotationQuaternion(const SVector4& qut)
{
	return XMMatrixRotationQuaternion(qut.Convert());
}

Matrix Matrix::StaticRotationAxis(const SVector3& Axis, float Angle)
{
	return XMMatrixRotationAxis(Axis.Convert(), SMathUtils::DegreesToRadians(Angle));
}

Matrix Matrix::StaticTranslation(const SVector3& _v)
{
	return XMMatrixTranslation(_v.x, _v.y, _v.z);
}

Matrix Matrix::StaticTranslation(float x, float y, float z)
{
	return XMMatrixTranslation(x,y,z);
}

Matrix Matrix::StaticTranslation(const SVector2& _v)
{
	return XMMatrixTranslation(_v.x, _v.y, 0.0f);
}

Matrix Matrix::StaticTranslation(float x, float y)
{
	return XMMatrixTranslation(x, y, 0.0f);
}

Matrix Matrix::NormalMatrix(const Matrix& world)
{
	Matrix nrm = world;

	//nrm.v[3] = SVector4(0, 0, 0, 1);
	nrm.m.r[3].m128_f32[0] = nrm.m.r[3].m128_f32[1] = nrm.m.r[3].m128_f32[2] = 0;
	nrm.m.r[3].m128_f32[3] = 1;
	XMVECTOR Det = XMMatrixDeterminant(nrm.m);
	nrm = XMMatrixInverse(&Det, nrm.m);
	nrm = XMMatrixTranspose(nrm.m);
	return nrm;
}

struct SVector3 Matrix::GetScale() const
{
	const int32 xs = (SMathUtils::Sign<int32>((int32)(m00 * m01 * m02 * m03)) < 0) ? -1 : 1;
	const int32 ys = (SMathUtils::Sign<int32>((int32)(m10 * m11 * m12 * m13)) < 0) ? -1 : 1;
	const int32 zs = (SMathUtils::Sign<int32>((int32)(m20 * m21 * m22 * m23)) < 0) ? -1 : 1;

	return SVector3(
		static_cast<float>(xs) * ::sqrt(m00 * m00 + m01 * m01 + m02 * m02),
		static_cast<float>(ys) * ::sqrt(m10 * m10 + m11 * m11 + m12 * m12),
		static_cast<float>(zs) * ::sqrt(m20 * m20 + m21 * m21 + m22 * m22)
	);

}

struct SVector4 Matrix::GetRotationQuaternion() const
{
	const SVector3 scale = GetScale();

	// Avoid division by zero (we'll divide to remove scaling)
	if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) { return SVector4(0, 0, 0, 1); }

	// Extract rotation and remove scaling
	Matrix normalized;
	normalized.m00 = m00 / scale.x; normalized.m01 = m01 / scale.x; normalized.m02 = m02 / scale.x; normalized.m03 = 0.0f;
	normalized.m10 = m10 / scale.y; normalized.m11 = m11 / scale.y; normalized.m12 = m12 / scale.y; normalized.m13 = 0.0f;
	normalized.m20 = m20 / scale.z; normalized.m21 = m21 / scale.z; normalized.m22 = m22 / scale.z; normalized.m23 = 0.0f;
	normalized.m30 = 0; normalized.m31 = 0; normalized.m32 = 0; normalized.m33 = 1.0f;

	return RotationMatrixToQuaternion(normalized);
}

struct SVector3 Matrix::GetTranslation() const
{
	return SVector3(m03, m13, m23);
}

struct SVector4 Matrix::RotationMatrixToQuaternion(const Matrix& mRot)
{
	SVector4 quaternion;
	float sqrt;
	float half;
	const float scale = mRot.m00 + mRot.m11 + mRot.m22;

	if (scale > 0.0f)
	{
		sqrt = ::sqrt(scale + 1.0f);
		quaternion.w = sqrt * 0.5f;
		sqrt = 0.5f / sqrt;

		quaternion.x = (mRot.m12 - mRot.m21) * sqrt;
		quaternion.y = (mRot.m20 - mRot.m02) * sqrt;
		quaternion.z = (mRot.m01 - mRot.m10) * sqrt;

		return quaternion;
	}
	if ((mRot.m00 >= mRot.m11) && (mRot.m00 >= mRot.m22))
	{
		sqrt = ::sqrt(1.0f + mRot.m00 - mRot.m11 - mRot.m22);
		half = 0.5f / sqrt;

		quaternion.x = 0.5f * sqrt;
		quaternion.y = (mRot.m01 + mRot.m10) * half;
		quaternion.z = (mRot.m02 + mRot.m20) * half;
		quaternion.w = (mRot.m12 - mRot.m21) * half;

		return quaternion;
	}
	if (mRot.m11 > mRot.m22)
	{
		sqrt = ::sqrt(1.0f + mRot.m11 - mRot.m00 - mRot.m22);
		half = 0.5f / sqrt;

		quaternion.x = (mRot.m10 + mRot.m01) * half;
		quaternion.y = 0.5f * sqrt;
		quaternion.z = (mRot.m21 + mRot.m12) * half;
		quaternion.w = (mRot.m20 - mRot.m02) * half;

		return quaternion;
	}
	sqrt = ::sqrt(1.0f + mRot.m22 - mRot.m00 - mRot.m11);
	half = 0.5f / sqrt;

	quaternion.x = (mRot.m20 + mRot.m02) * half;
	quaternion.y = (mRot.m21 + mRot.m12) * half;
	quaternion.z = 0.5f * sqrt;
	quaternion.w = (mRot.m01 - mRot.m10) * half;

	return quaternion;
}

void Matrix::ToImGuizmo(float* dest, const Matrix& src)
{
	for (auto row = 0; row < 4; row++)
	{
		for (auto col = 0; col < 4; col++)
			dest[row * 4 + col] = src.Data()[col * 4 + row];
	}
}

void Matrix::FromImGuizmo(Matrix& dest, float* src)
{
	for (auto row = 0; row < 4; row++)
	{
		for (auto col = 0; col < 4; col++)
			(&dest.m00)[col * 4 + row] = src[row * 4 + col];
	}
}



const Matrix& Matrix::operator*=(const XMMATRIX& _m)
{
	m *= _m;
	return *this;
}

const Matrix& Matrix::operator*=(const Matrix& _m)
{
	m *= _m.m;
	return *this;
}

Matrix Matrix::operator*(const XMMATRIX& _m) const
{
	return Matrix(m * _m);
}

Matrix Matrix::operator*(const Matrix& _m) const
{
	return Matrix(m * _m.m);
}

Matrix& Matrix::operator=(SVector4 _v[4])
{
	::memcpy(v, _v, sizeof(SVector4) * 4);
	return *this;
}

Matrix& Matrix::operator=(const XMMATRIX& _m)
{
	m = _m;
	return *this;
}

Matrix& Matrix::operator=(const Matrix& _m)
{
	m = _m.m;
	return *this;
}

SVector4& Matrix::operator[](int Index)
{
	return v[Index];
}
