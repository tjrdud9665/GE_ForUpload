#pragma once

#include "Matrix.h"

struct SVector3
{
	float x;
	float y;
	float z;

	/** A zero vector (0,0,0) */
	static  const SVector3 ZeroVector;

	/** One vector (1,1,1) */
	static const SVector3 OneVector;

	/** Up Vector (0,1,0) */
	static const SVector3 UpVector;

	/**  down vector (0,-1,0) */
	static const SVector3 DownVector;

	/**  forward vector (0,0,1) */
	static const SVector3 ForwardVector;

	/**  backward vector (0,0,-1) */
	static const SVector3 BackwardVector;

	/**  right vector (0,1,0) */
	static const SVector3 RightVector;

	/**  left vector (-1,0,0) */
	static const SVector3 LeftVector;

	/** Unit X axis vector (1,0,0) */
	static const SVector3 XAxisVector;

	/** Unit Y axis vector (0,1,0) */
	static const SVector3 YAxisVector;

	/** Unit Z axis vector (0,0,1) */
	static const SVector3 ZAxisVector;

	static const SVector3 Axis[EAxis::MAX];


	SVector3();
	SVector3(float _f);
	SVector3(float _x, float _y, float _z);
	SVector3(const SVector3& v);
	SVector3(const struct SVector2& v,float _z);
	SVector3(const struct SVector2& v);
	SVector3(const struct SVector4& v);
	SVector3(const XMVECTOR& v);

	// operator =
	SVector3& operator = (const SVector3& v);
	SVector3& operator = (const XMVECTOR& v);
	SVector3& operator = (float f);
	



	// operator ==, !=
	bool operator == (const SVector3& v)	const;
	bool operator == (const XMVECTOR& v)	const;
	bool operator != (const SVector3& v)	const;
	bool operator != (const XMVECTOR& v)	const;

	

	// operator +
	SVector3 operator + (const SVector3& v)	const;
	SVector3 operator + (const XMVECTOR& v)	const;
	SVector3 operator + (const float f)	const;

	// operator +=
	SVector3& operator += (const SVector3& v);
	SVector3& operator += (const XMVECTOR& v);
	SVector3& operator += (const float f);

	// operator -
	SVector3 operator - (const SVector3& v)	const;
	SVector3 operator - (const XMVECTOR& v)	const;
	SVector3 operator - (const float f)	const;

	// operator -=
	SVector3& operator -= (const SVector3& v);
	SVector3& operator -= (const XMVECTOR& v);
	SVector3& operator -= (const float f);

	// operator *
	SVector3 operator * (const SVector3& v)	const;
	SVector3 operator * (const XMVECTOR& v)	const;
	SVector3 operator * (const float f)	const;

	// operator *=
	SVector3& operator *= (const SVector3& v);
	SVector3& operator *= (const XMVECTOR& v);
	SVector3& operator *= (const float f);

	// operator /
	SVector3 operator / (const SVector3& v)	const;
	SVector3 operator / (const XMVECTOR& v)	const;
	SVector3 operator / (const float f)	const;

	// operator /=
	SVector3& operator /= (const SVector3& v);
	SVector3& operator /= (const XMVECTOR& v);
	SVector3& operator /= (const float f);

	// operator ++
	const SVector3& operator ++ ();
	const SVector3& operator ++ (int);

	// operator --
	const SVector3& operator -- ();
	const SVector3& operator -- (int);
	

	static float DistSquared(const SVector3& V1, const SVector3& V2);
	static float Distance(const SVector3& V1, const SVector3& V2);


	SVector3 operator^(const SVector3& V) const;
	static SVector3 CrossProduct(const SVector3& A, const SVector3& B);

	float operator|(const SVector3& V) const;
	static float DotProduct(const SVector3& A, const SVector3& B);
		
	
	float SizeSquared() const;
	float Size() const;



	float* Data() { return &x; }
	const float* Data() const { return &x; }

	float& operator[](int32 Index);

	float operator[](int32 Index)const;


	SVector3 GetSafeNormal(float Tolerance = SMALL_NUMBER) const;
	bool Normalize(float Tolerance = SMALL_NUMBER);

	SVector3 GetReciprocal(float Tolerance = SMALL_NUMBER) const;


	XMVECTOR Convert()	const;
	void Convert(const XMVECTOR& v);

	SVector3 ConvertAngle() const;

	// w = 0
	SVector3 TransformNormal(const Matrix& m)	const;

	// w = 1
	SVector3 TransformCoord(const Matrix& m)	const;


	SVector3 RotateAngleAxis(const float AngleDeg, const SVector3& Axis);

	static SVector3 VInterpTo(const SVector3& Current, const SVector3& Target, float DeltaTime, float InterpSpeed);


	static SVector3 VLerp(const SVector3& A, const SVector3& B, float Alpha)
	{
		return A + ((B - A) * Alpha);
	}


	
};

static SVector3 operator*(const float f, const SVector3& v)
{
	return v * f;
}

static SVector3 operator-(const SVector3& v)
{
	return -1.0f * v;
}