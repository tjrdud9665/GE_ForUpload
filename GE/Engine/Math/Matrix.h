#pragma once

#include "Vector4.h"


/*
* Matrix Of Scale
scaleX, 0,		0,		0,
0,		scaleY, 0,		0,
0,		0,		ScaleZ,	0,
0,		0,		0,		1

* Matrix Of Translation
* 
1,		0,		0,		0,
0,		1,		0,		0,
0,		0,		1,		0,
t.x,	t.y,	t.z,	1.0f

//Rotations..

*/

union __declspec(align(16)) Matrix
{
	XMMATRIX	m;

	struct
	{
		float m00, m10 , m20 , m30 ;
		float m01, m11 , m21 , m31 ;
		float m02, m12 , m22 , m32 ;
		float m03, m13 , m23 , m33 ;
	};

	SVector4	v[4];

	Matrix();
	Matrix(const Matrix& _m);
	Matrix(const XMMATRIX& _m);
	Matrix(SVector4 _v[4]);

	Matrix(
		float _m00, float _m01, float _m02, float _m03,
		float _m10, float _m11, float _m12, float _m13,
		float _m20, float _m21, float _m22, float _m23,
		float _m30, float _m31, float _m32, float _m33)
	{
		m00 = _m00; m01 = _m01; m02 = _m02; m03 = _m03;
		m10 = _m10; m11 = _m11; m12 = _m12; m13 = _m13;
		m20 = _m20; m21 = _m21; m22 = _m22; m23 = _m23;
		m30 = _m30; m31 = _m31; m32 = _m32; m33 = _m33;
	}


	SVector4& operator [] (int Index);
	Matrix& operator = (const Matrix& _m);
	Matrix& operator = (const XMMATRIX& _m);
	Matrix& operator = (SVector4 _v[4]);

	Matrix operator * (const Matrix& _m)	const;
	Matrix operator * (const XMMATRIX& _m)	const;

	const Matrix& operator *= (const Matrix& _m);
	const Matrix& operator *= (const XMMATRIX& _m);

	void Identity();
	void Transpose();
	void Inverse();
	

	float* Data() { return &(v[0].x); }
	const float* Data() const { return &(v[0].x); }

	bool hasNAN()const;

	void Scaling(const struct SVector3& _v);
	void Scaling(float x, float y, float z);
	void Scaling(const SVector2& _v);
	void Scaling(float x, float y);

	void Rotation(const struct SVector3& _v);
	void Rotation(float x, float y, float z);

	void RotationX(float x);
	void RotationY(float y);
	void RotationZ(float z);

	void RotationQuaternion(const SVector4& qut);

	void RotationAxis(const  SVector3& Axis, float Angle);

	void Translation(const struct SVector3& _v);
	void Translation(float x, float y, float z);
	void Translation(const SVector2& _v);
	void Translation(float x, float y);

	void DecomposeMatrix(SVector3& OutScale, SVector4& OutQuat, SVector3& OutLoc) const;

	struct SVector3 GetScaledAxis(EAxis InAxis) const;

	struct SVector3 GetUnitAxis(EAxis InAxis) const;


	struct SVector3 ExtractScaling(float Tolerance = SMALL_NUMBER);


	static Matrix StaticIdentity();
	static Matrix StaticTranspose(const Matrix& _m);
	static Matrix StaticInverse(const Matrix& _m);

	static Matrix StaticScaling(const  SVector3& _v);
	static Matrix StaticScaling(float x, float y, float z);
	static Matrix StaticScaling(const SVector2& _v);
	static Matrix StaticScaling(float x, float y);

	static Matrix StaticRotation(const  SVector3& _v);
	static Matrix StaticRotation(float x, float y, float z);

	static Matrix StaticRotationX(float x);
	static Matrix StaticRotationY(float y);
	static Matrix StaticRotationZ(float z);

	static Matrix StaticRotationQuaternion(const SVector4& qut);
	static Matrix StaticRotationAxis(const struct SVector3& Axis, float Angle);
	static Matrix StaticTranslation(const struct SVector3& _v);
	static Matrix StaticTranslation(float x, float y, float z);
	static Matrix StaticTranslation(const SVector2& _v);
	static Matrix StaticTranslation(float x, float y);

	static Matrix NormalMatrix(const Matrix& world);

	struct SVector3 GetScale() const;

	struct SVector4 GetRotationQuaternion() const;

	struct SVector3	 GetTranslation() const;
	
	static struct SVector4 RotationMatrixToQuaternion(const Matrix& mRot);

	static void ToImGuizmo(float* dest, const Matrix& src);

	static void FromImGuizmo(Matrix& dest, float* src);
};

