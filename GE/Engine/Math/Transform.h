#pragma once


struct STransform
{
	SVector3	Translation;

	SVector4	Rotation;
		
	SVector3	Scale3D;

	STransform()
		:Rotation(SVector4(0,0,0,1.0f))
		,Translation(SVector3(0.0f))
		,Scale3D(SVector3(1.0f))
	{
	}

	STransform(const SVector3& InTranslation)
		:Rotation(SVector4(0, 0, 0, 1.0f))
		,Translation(InTranslation)
		,Scale3D(SVector3(1.0f))
	{

	}

	STransform(const SVector4& InRotQut , const SVector3& InTranslation, const SVector3& InScale3D)
		:Rotation(InRotQut)
		,Translation(InTranslation)
		,Scale3D(InScale3D)
	{

	}

	bool Equals(const STransform& Other,float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return SMathUtils::Equals(Other.Translation.x, Translation.x) && SMathUtils::Equals(Other.Translation.y, Translation.y) && SMathUtils::Equals(Other.Translation.z, Translation.z) &&
			SMathUtils::Equals(Other.Rotation.x, Rotation.x) && SMathUtils::Equals(Other.Rotation.y, Rotation.y) && SMathUtils::Equals(Other.Rotation.z, Rotation.z) && SMathUtils::Equals(Other.Rotation.w, Rotation.w)
			&& SMathUtils::Equals(Other.Scale3D.x, Scale3D.x) && SMathUtils::Equals(Other.Scale3D.y, Scale3D.y) && SMathUtils::Equals(Other.Scale3D.z, Scale3D.z);		
	}

	STransform operator*(const STransform& Other) const
	{
		STransform Output;
		Multiply(&Output, this, &Other);
		return Output;
	}

	static void Multiply(STransform* OutTransform, const STransform* A , const STransform* B)
	{
		const SVector4 QuatA = A->Rotation;
		const SVector4 QuatB = B->Rotation;
		const SVector3 TranslateA = A->Translation;
		const SVector3 TranslateB = B->Translation;
		const SVector3 ScaleA = A->Scale3D;
		const SVector3 ScaleB = B->Scale3D;

		// RotationResult = B.Rotation * A.Rotation
		OutTransform->Rotation = SVector4::VectorQuaternionMultiply2(QuatB, QuatA);

		// TranslateResult = B.Rotate(B.Scale * A.Translation) + B.Translate
		const SVector3 ScaledTransA = TranslateA * ScaleB;					
		const SVector3 RotatedTranslate = SVector4::VectorQuaternionRotateVector(QuatB, ScaledTransA);
		OutTransform->Translation = (RotatedTranslate + TranslateB);

		// ScaleResult = Scale.B * Scale.A
		OutTransform->Scale3D = (ScaleA * ScaleB);;
	}	

	STransform Inverted() const
	{
		STransform result;
		
		result.Rotation = Rotation.Inverse();
		result.Translation = result.Rotation.Rotate(-Translation / Scale3D);
		result.Scale3D = GetSafeScaleReciprocal(Scale3D);
		return result;
	}

	
	STransform GetRelativeTransform(const STransform& Other) const
	{
		STransform Result;

		SVector3 SafeRecipScale3D = Other.Scale3D.GetReciprocal();				
		Result.Scale3D = Scale3D * SafeRecipScale3D;
	
		SVector4	Inverse = Other.Rotation.Inverse();			
		Result.Rotation = Inverse * Rotation;
		Result.Translation = (Inverse * (Translation - Other.Translation)) * (SafeRecipScale3D);


		return Result;
	}
	//  translation -> rotation -> scale
	SVector3 InverseTransformPosition(const SVector3& V) const
	{
		// (V-Translation)
		const SVector4 W0 = V;		

		SVector4 TranslatedVec = W0 - Translation;
		
		// ( Rotation.Inverse() * (V-Translation) )
		const SVector4  VR = SMathUtils::VectorQuaternionInverseRotateVector(Rotation, TranslatedVec);

		const SVector3 Result = VR * Scale3D.GetReciprocal();

		return Result;
	}


	Matrix	ToMatrixWithScale() const
	{
		Matrix OutMatrix;
		

		OutMatrix.m03 = Translation.x;
		OutMatrix.m13 = Translation.y;
		OutMatrix.m23 = Translation.z;

		const float x2 = Rotation.x + Rotation.x;
		const float y2 = Rotation.y + Rotation.y;
		const float z2 = Rotation.z + Rotation.z;
		{
			const float xx2 = Rotation.x * x2;
			const float yy2 = Rotation.y * y2;
			const float zz2 = Rotation.z * z2;

			OutMatrix.m00 = (1.0f - (yy2 + zz2)) * Scale3D.x;
			OutMatrix.m11  = (1.0f - (xx2 + zz2)) * Scale3D.y;
			OutMatrix.m22 = (1.0f - (xx2 + yy2)) * Scale3D.z;
		}
		{
			const float yz2 = Rotation.y * z2;
			const float wx2 = Rotation.w * x2;

			OutMatrix.m12 = (yz2 - wx2) * Scale3D.z;
			OutMatrix.m21 = (yz2 + wx2) * Scale3D.y;
		}
		{
			const float xy2 = Rotation.x * y2;
			const float wz2 = Rotation.w * z2;

			OutMatrix.m01 = (xy2 - wz2) * Scale3D.y;
			OutMatrix.m10 = (xy2 + wz2) * Scale3D.x;
		}
		{
			const float xz2 = Rotation.x * z2;
			const float wy2 = Rotation.w * y2;

			OutMatrix.m02 = (xz2 + wy2) * Scale3D.z;
			OutMatrix.m20 = (xz2 - wy2) * Scale3D.x;
		}

		OutMatrix.m30 = 0.0f;
		OutMatrix.m31 = 0.0f;
		OutMatrix.m32 = 0.0f;
		OutMatrix.m33 = 1.0f;

		return OutMatrix;
	}

	Matrix	ToMatrixNoScale() const
	{
		Matrix OutMatrix;

		OutMatrix.m03 = Translation.x;
		OutMatrix.m13 = Translation.y;
		OutMatrix.m23 = Translation.z;

		const float x2 = Rotation.x + Rotation.x;
		const float y2 = Rotation.y + Rotation.y;
		const float z2 = Rotation.z + Rotation.z;
		{
			const float xx2 = Rotation.x * x2;
			const float yy2 = Rotation.y * y2;
			const float zz2 = Rotation.z * z2;

			OutMatrix.m00 = (1.0f - (yy2 + zz2));
			OutMatrix.m11 = (1.0f - (xx2 + zz2));
			OutMatrix.m22 = (1.0f - (xx2 + yy2));
		}
		{
			const float yz2 = Rotation.y * z2;
			const float wx2 = Rotation.w * x2;

			OutMatrix.m12 = (yz2 - wx2);
			OutMatrix.m21 = (yz2 + wx2);
		}
		{
			const float xy2 = Rotation.x * y2;
			const float wz2 = Rotation.w * z2;

			OutMatrix.m01 = (xy2 - wz2);
			OutMatrix.m10 = (xy2 + wz2);
		}
		{
			const float xz2 = Rotation.x * z2;
			const float wy2 = Rotation.w * y2;

			OutMatrix.m02 = (xz2 + wy2);
			OutMatrix.m20 = (xz2 - wy2);
		}

		OutMatrix.m30 = 0.0f;
		OutMatrix.m31 = 0.0f;
		OutMatrix.m32 = 0.0f;
		OutMatrix.m33 = 1.0f;

		return OutMatrix;
	}

	void FromMatrix(const Matrix& InMatrix)
	{
		Matrix matrix = InMatrix;
		matrix.DecomposeMatrix(OUT Scale3D, OUT Rotation, OUT Translation);

	}

	SVector3 GetSafeScaleReciprocal(const SVector3& InScale, float Tolerance = SMALL_NUMBER) const
	{
		SVector3 SafeReciprocalScale;
		if (::abs(InScale.x) <= Tolerance)
		{
			SafeReciprocalScale.x = 0.f;
		} else
		{
			SafeReciprocalScale.x = 1 / InScale.x;
		}

		if (::abs(InScale.y) <= Tolerance)
		{
			SafeReciprocalScale.y = 0.f;
		} else
		{
			SafeReciprocalScale.y = 1 / InScale.y;
		}

		if (::abs(InScale.z) <= Tolerance)
		{
			SafeReciprocalScale.z = 0.f;
		} else
		{
			SafeReciprocalScale.z = 1 / InScale.z;
		}

		return SafeReciprocalScale;
	}

	static const STransform Identity;

};

