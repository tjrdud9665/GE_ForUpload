#pragma once

//= INCLUDES ==========================
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
// Hide warnings which belong to Bullet
#pragma warning(push, 0) 
#include "LinearMath/btQuaternion.h"
#pragma warning(pop)
//=====================================

inline SVector3 ToVector3(const btVector3& vector)
{
	return SVector3(vector.getX(), vector.getY(), vector.getZ());
}

inline SVector4 ToVector4(const btVector3& vector)
{
	return SVector4(vector.getX(), vector.getY(), vector.getZ(), 1.0f);
}

inline btVector3 ToBtVector3(const SVector3& vector)
{
	return btVector3(vector.x, vector.y, vector.z);
}

inline btQuaternion ToBtQuaternion(const SVector4& quaternion)
{
	return btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

inline SVector4 ToQuaternion(const btQuaternion& quaternion)
{
	return SVector4(quaternion.getX(), quaternion.getY(), quaternion.getZ(), quaternion.getW());
}

inline btTransform ToBtTransform(const STransform& NoScaleTransform)
{
	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin(ToBtVector3(NoScaleTransform.Translation));
	btTrans.setRotation(ToBtQuaternion(NoScaleTransform.Rotation));

	return btTrans;

}
