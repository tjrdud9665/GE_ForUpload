#pragma once

struct SMathUtils
{
	static float Random(float Min, float Max)
	{

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(Min, Max);

		return  (float)dist(gen);
	}

	static  void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (INV_PI * 0.5f) * Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = Value - (2.0f * PI) * quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign * p;
	}


	static float  DegreesToRadians(float const& DegVal)
	{
		return DegVal * (PI / 180.f);
	}

	static float  RadiansToDegrees(float const& RadVal)
	{
		return RadVal * (180.f / PI);
	}

	static  float InvSqrt(float F)
	{
		return 1.0f / ::sqrt(F);
	}

	static float Square(const float value)
	{
		return value * value;
	}

	template< class T >
	static  T Clamp(const T X, const T Min, const T Max)
	{
		return X <= Min ? Min : X <= Max ? X : Max;		    
	}

	static float InterpTo(float Current, float Target, float DeltaTime, float InterpSpeed)
	{
		// If no interp speed, jump to target value
		if (InterpSpeed <= 0.f)
		{
			return Target;
		}

		// Distance to reach
		const float Dist = Target - Current;

		// If distance is too small, just set the desired location
		if (SMathUtils::Square(Dist) < SMALL_NUMBER)
		{
			return Target;
		}

		// Delta Move, Clamp so we do not over shoot.
		const float DeltaMove = Dist * SMathUtils::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

		return Current + DeltaMove;
	}


	static float Lerp(float A, float B, float Alpha)
	{
		return A + Alpha * (B - A);
	}

	template <typename T>
	static int32 Sign(T x) { return (static_cast<T>(0) < x) - (x < static_cast<T>(0)); }

		
	static bool Equals(float L, float R, float Tolerance = KINDA_SMALL_NUMBER)
	{
		return ::abs(L - R) <= Tolerance;
	}


	static struct SVector4 VectorQuaternionInverseRotateVector(const struct SVector4& Quat, const struct SVector4& W0);

	static struct SVector4 EulerToQuat(const struct SVector3& InEulerRotation);

	static float NormalizeAngleAxis(struct SVector3& InRotator, float AngleDegree);

	static float FastAsin(float val);

	/**
	 * Return Euler Degree.
	 */
	static struct SVector3 FindLookAtRotation(const struct SVector3& Start, const struct SVector3& Target);

};


