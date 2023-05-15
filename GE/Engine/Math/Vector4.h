#pragma once

struct SVector4
{
	float x;
	float y;
	float z;
	float w;

	SVector4();
	SVector4(float _f);
	SVector4(float _x, float _y, float _z, float _w);
	SVector4(const SVector4& v);
	SVector4(const struct SVector3& v);
	SVector4(const XMVECTOR& v);

	SVector4(const struct SVector3& v, float _w);

	// operator =
	SVector4& operator = (const SVector4& v);
	SVector4& operator = (const XMVECTOR& v);
	SVector4& operator = (float f);

	// operator ==, !=
	bool operator == (const SVector4& v)	const;
	bool operator == (const XMVECTOR& v)	const;
	bool operator != (const SVector4& v)	const;
	bool operator != (const XMVECTOR& v)	const;

	float& operator [] (int Index);
	const float* Data() const;

	// operator +
	SVector4 operator + (const SVector4& v)	const;
	SVector4 operator + (const XMVECTOR& v)	const;
	SVector4 operator + (const float f)	const;

	// operator +=
	SVector4& operator += (const SVector4& v);
	SVector4& operator += (const XMVECTOR& v);
	SVector4& operator += (const float f);

	// operator -
	SVector4 operator - (const SVector4& v)	const;
	SVector4 operator - (const XMVECTOR& v)	const;
	SVector4 operator - (const float f)	const;

	// operator -=
	SVector4& operator -= (const SVector4& v);
	SVector4& operator -= (const XMVECTOR& v);
	SVector4& operator -= (const float f);

	// operator *
	SVector4 operator * (const SVector4& v)	const;
	SVector4 operator * (const XMVECTOR& v)	const;
	SVector4 operator * (const float f)	const;

	// operator *=
	SVector4& operator *= (const SVector4& v);
	SVector4& operator *= (const XMVECTOR& v);
	SVector4& operator *= (const float f);

	// operator /
	SVector4 operator / (const SVector4& v)	const;
	SVector4 operator / (const XMVECTOR& v)	const;
	SVector4 operator / (const float f)	const;

	// operator /=
	SVector4& operator /= (const SVector4& v);
	SVector4& operator /= (const XMVECTOR& v);
	SVector4& operator /= (const float f);

	// operator ++
	const SVector4& operator ++ ();
	const SVector4& operator ++ (int);

	// operator --
	const SVector4& operator -- ();
	const SVector4& operator -- (int);

	XMVECTOR Convert()	const;
	void Convert(const XMVECTOR& v);

	SVector4	Inverse() const;

	bool HasNan() const;
		

	static SVector4 VLerp(const SVector4& A, const SVector4& B, float Alpha)
	{
		return A + ((B - A) * Alpha);
	}

	static SVector4 VectorQuaternionMultiply2(const SVector4& QA, const SVector4& QB);

	static SVector4 VectorQuaternionRotateVector(const SVector4& QA, const SVector3& VectorW0);

	struct SVector3 ToEulerAngles() const;

	struct SVector3 Rotate(const SVector3& v) const;

	struct SVector4 QuaternionConjugated() const;
	
	
	SVector4 GetSafeNormal(float Tolerance = SMALL_NUMBER) const;
	bool Normalize(float Tolerance = SMALL_NUMBER);

	static const SVector4	ZeroVector;
	static const SVector4	Black;
	static const SVector4	White;
	static const SVector4	Red;
	static const SVector4	Green;
	static const SVector4	Blue;
};

