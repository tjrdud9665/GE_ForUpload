#pragma once


enum EPlaneset
{
	PL_RIGHT = 0,
	PL_LEFT,
	PL_TOP,
	PL_BOTTOM,
	PL_FAR,
	PL_NEAR
};

// src: http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdfe
	// gets the frustum planes based on @projectionTransformation. if:
	//
	// - @projectionTransformation is proj			matrix ->  view space plane equations
	// - @projectionTransformation is viewProj		matrix -> world space plane equations
	// - @projectionTransformation is worldViewProj matrix -> model space plane equations
	// 

struct SFrustumPlaneset
{	
	// plane equations: aX + bY + cZ + d = 0
	SVector4 abcd[6];	// r, l, t, b, n, f	
	

	inline static SFrustumPlaneset ExtractFromMatrix(const Matrix& m)
	{
		
		SFrustumPlaneset viewPlanes;
		viewPlanes.abcd[EPlaneset::PL_RIGHT] = SVector4(
			m.m.r[0].m128_f32[3] - m.m.r[0].m128_f32[0],
			m.m.r[1].m128_f32[3] - m.m.r[1].m128_f32[0],
			m.m.r[2].m128_f32[3] - m.m.r[2].m128_f32[0],
			m.m.r[3].m128_f32[3] - m.m.r[3].m128_f32[0]
		);
		viewPlanes.abcd[EPlaneset::PL_LEFT] = SVector4(
			m.m.r[0].m128_f32[3] + m.m.r[0].m128_f32[0],
			m.m.r[1].m128_f32[3] + m.m.r[1].m128_f32[0],
			m.m.r[2].m128_f32[3] + m.m.r[2].m128_f32[0],
			m.m.r[3].m128_f32[3] + m.m.r[3].m128_f32[0]
		);
		viewPlanes.abcd[EPlaneset::PL_TOP] = SVector4(
			m.m.r[0].m128_f32[3] - m.m.r[0].m128_f32[1],
			m.m.r[1].m128_f32[3] - m.m.r[1].m128_f32[1],
			m.m.r[2].m128_f32[3] - m.m.r[2].m128_f32[1],
			m.m.r[3].m128_f32[3] - m.m.r[3].m128_f32[1]
		);
		viewPlanes.abcd[EPlaneset::PL_BOTTOM] = SVector4(
			m.m.r[0].m128_f32[3] + m.m.r[0].m128_f32[1],
			m.m.r[1].m128_f32[3] + m.m.r[1].m128_f32[1],
			m.m.r[2].m128_f32[3] + m.m.r[2].m128_f32[1],
			m.m.r[3].m128_f32[3] + m.m.r[3].m128_f32[1]
		);
		viewPlanes.abcd[EPlaneset::PL_FAR] = SVector4(
			m.m.r[0].m128_f32[3] - m.m.r[0].m128_f32[2],
			m.m.r[1].m128_f32[3] - m.m.r[1].m128_f32[2],
			m.m.r[2].m128_f32[3] - m.m.r[2].m128_f32[2],
			m.m.r[3].m128_f32[3] - m.m.r[3].m128_f32[2]
		);
		viewPlanes.abcd[EPlaneset::PL_NEAR] = SVector4(
			m.m.r[0].m128_f32[2],
			m.m.r[1].m128_f32[2],
			m.m.r[2].m128_f32[2],
			m.m.r[3].m128_f32[2]
		);
		return viewPlanes;
	}
};

