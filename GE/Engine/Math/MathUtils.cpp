#include "EnginePCH.h"
#include "MathUtils.h"


// do backward operation when inverse, translation -> rotation -> scale
struct SVector4 SMathUtils::VectorQuaternionInverseRotateVector(const struct SVector4& Quat, const struct SVector4& W0)
{
	SVector4	invQ = Quat;	
	invQ  = DirectX::XMQuaternionInverse(Quat.Convert());

	const SVector4 temp  = DirectX::XMQuaternionMultiply(W0.Convert(), invQ.Convert());
	const SVector4 Rotated = DirectX::XMQuaternionMultiply(Quat.Convert(),temp.Convert() );

	return Rotated;

}

struct SVector4 SMathUtils::EulerToQuat(const struct SVector3& InEulerRotation)
{
	const float halfRoll = InEulerRotation.x * 0.5f;
	const float halfPitch = InEulerRotation.y * 0.5f;
	const float halfYaw = InEulerRotation.z * 0.5f;

	const float sinRoll = sin(halfRoll);
	const float cosRoll = cos(halfRoll);
	const float sinPitch = sin(halfPitch);
	const float cosPitch = cos(halfPitch);
	const float sinYaw = sin(halfYaw);
	const float cosYaw = cos(halfYaw);

	return SVector4(
		cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
		sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
		cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
		cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
	);
}


float SMathUtils::NormalizeAngleAxis(struct SVector3& InRotator, float AngleDegree)
{
	float Ret = AngleDegree  / 360.0f;

	if (Ret < 0.f)
	{
		// shift to [0,360) range
		Ret += 360.f;
	}
	if (Ret > 180.f)
	{
		// shift to (-180,180]
		Ret -= 360.f;
	}

	return Ret;

}

#define FASTASIN_HALF_PI 1.5707963050f

float SMathUtils::FastAsin(float val)
{
	
	// Clamp input to [-1,1].
	bool nonnegative = (val >= 0.0f);
	float x = ::abs(val);
	float omx = 1.0f - x;
	if (omx < 0.0f)
	{
		omx = 0.0f;
	}
	float root = ::sqrt(omx);
	// 7-degree minimax approximation
	float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
	result *= root;  // acos(|x|)
	// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
	return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
}

struct SVector3 SMathUtils::FindLookAtRotation(const struct SVector3& Start, const struct SVector3& Target)
{
	SVector3 rotAx = (Target - Start).GetSafeNormal();

	const SVector3 zAxis = rotAx;
	const SVector3 xAxis = SVector3::CrossProduct(SVector3::UpVector, zAxis).GetSafeNormal();
	const SVector3 yAxis = SVector3::CrossProduct(zAxis, xAxis).GetSafeNormal();

	
	Matrix mat = Matrix(xAxis.x, yAxis.x, zAxis.x, 0,
						xAxis.y, yAxis.y, zAxis.y, 0,
						xAxis.z, yAxis.z, zAxis.z, 0,
						0, 0, 0, 1.0f);

	
	SVector3 discard;
	SVector4 quat;
	mat.DecomposeMatrix(discard, quat, discard);
	return quat.ToEulerAngles();	


}

