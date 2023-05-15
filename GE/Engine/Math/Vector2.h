#pragma once
#include "MathUtils.h"

struct SVector2
{
	float x;
	float y;

	SVector2();
	SVector2(int32 _x, int32 _y);
	SVector2(int32 _InI);

	SVector2(uint32 _x,uint32 _y);
	SVector2(uint32 _InI);	

	SVector2(float _Inf);
	SVector2(float _x,float _y);
	SVector2(const SVector2& v);
	SVector2(const XMVECTOR& v);

	SVector2(const struct SVector3& v);

	// operator =
	SVector2& operator = (const struct SVector3& v);
	SVector2& operator = (const SVector2& v);
	SVector2& operator = (const XMVECTOR& v);
	SVector2& operator = (float f);


	// operator ==, !=
	bool operator == (const SVector2& v)		const;
	bool operator == (const XMVECTOR& v)		const;
	bool operator != (const SVector2& v)		const;
	bool operator != (const XMVECTOR& v)		const;

	float& operator [] (int Index);
	float operator[] (int Index) const;



	// operator +
	SVector2 operator + (const SVector2& v)	const;
	SVector2 operator + (const XMVECTOR& v)	const;
	SVector2 operator + (const float f)		const;

	float operator^(const SVector2& V) const;
	float operator|(const SVector2& V) const;

	// operator +=
	SVector2& operator += (const SVector2& v);
	SVector2& operator += (const XMVECTOR& v);
	SVector2& operator += (const float f);

	// operator -
	SVector2 operator - (const SVector2& v)	const;
	SVector2 operator - (const XMVECTOR& v)	const;
	SVector2 operator - (const float f)		const;

	// operator -=
	SVector2& operator -= (const SVector2& v);
	SVector2& operator -= (const XMVECTOR& v);
	SVector2& operator -= (const float f);

	// operator *
	SVector2 operator * (const SVector2& v)	const;
	SVector2 operator * (const XMVECTOR& v)	const;
	SVector2 operator * (const float f)		const;

	// operator *=
	SVector2 operator *= (const SVector2& v);
	SVector2 operator *= (const XMVECTOR& v);
	SVector2 operator *= (const float f);

	// operator /
	SVector2 operator / (const SVector2& v)	const;
	SVector2 operator / (const XMVECTOR& v)	const;
	SVector2 operator / (const float f)		const;

	// operator /=
	SVector2& operator /= (const SVector2& v);
	SVector2& operator /= (const XMVECTOR& v);
	SVector2& operator /= (const float f);

	// operator ++
	const SVector2& operator ++ ();
	const SVector2& operator ++ (int);

	// operator --
	const SVector2& operator -- ();
	const SVector2& operator -- (int);

	float* Data() { return &x; }
	const float* Data() const { return &x; }
		
	float Size()	const;	

	float SizeSquared() const;

	void Normalize(float Tolerance = SMALL_NUMBER);

	SVector2 GetSafeNormal(float Tolerance = SMALL_NUMBER) const;

	SVector2	GetAbs() const;

	void		Abs();
	
	
	static float DistSquared(const SVector2& V1, const SVector2& V2);

	SVector2 GetRotated(const float AngleDeg) const;
	
	float GetAngle(const SVector2& v) const;

	static float GetAngle(const SVector2& v1 ,const SVector2& v2);


	static float Distance(const SVector2& V1, const SVector2& V2);	
	static float DotProduct(const SVector2& A, const SVector2& B);


	static SVector2 InterpTo(const SVector2& Current, const SVector2& Target, float DeltaTime, float InterpSpeed)
	{
		if (InterpSpeed <= 0.f)
		{
			return Target;
		}

		const SVector2 Dist = Target - Current;
		if (Dist.SizeSquared() < SMALL_NUMBER)
		{
			return Target;
		}

		const SVector2 DeltaMove = Dist * SMathUtils::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);
		return Current + DeltaMove;
	}

	static SVector2 VLerp(const SVector2& A, const SVector2& B, float Alpha)
	{
		return A + ((B - A) * Alpha);
	}


	XMVECTOR Convert()	const;
	void Convert(const XMVECTOR& v);


	static  const SVector2 ZeroVector;

	static  const SVector2 OneVector;

	static const SVector2 HalfVector;
	
	static const SVector2 RightVector;

	static const SVector2 LeftVector;

	static const SVector2 UpVector;

	static const SVector2 DownVector;

};

static SVector2 operator-(SVector2 rhs)
{
	rhs.x = rhs.x * -1.0f;
	rhs.y = rhs.y * -1.0f;

	return rhs;
}
