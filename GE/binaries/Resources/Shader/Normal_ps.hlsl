

struct PSIn
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 texCoord : TEXCOORD0;
};

cbuffer perObj
{
	float isNormalMap;
};

Texture2D texNormalMap;
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

inline float3 UnpackNormals(float2 uv, float3 worldNormal, float3 worldTangent)
{
	// uncompressed normal in tangent space
	float3 normalMapNormal = texNormalMap.Sample(samAnisotropic, uv).xyz;
	normalMapNormal = normalize(normalMapNormal * 2.0f - 1.0f);
	//float Nx = normalMapNormal.
	//normalMapNormal.y *= -1.0f;
	//normalMapNormal.z *= -1.0f;
	float3 N = normalize(worldNormal);
	float3 T = normalize(worldTangent);	// after interpolation, T and N might not be orthonormal
										// make sure T is orthonormal to N by subtracting off any component of T along direction N.
	T = normalize(worldTangent - dot(worldNormal, worldTangent) * worldNormal);
	float3 B = normalize(cross(T, N));
	float3x3 TBN = float3x3(T, B, N);
	//float3x3 TBN = float3x3(T, N, B);
	//float3x3 TBN = float3x3(-T, -B, -N);
	//float3x3 TBN = float3x3(float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1));

	float3 ret = normalize(mul(normalMapNormal, TBN));
	//float3 ret = normalize(mul(TBN, normalMapNormal));
	return ret;
	//return mul(normalMapNormal, TBN);
	//return TNormal;
	//return T;
}

float4 PSMain(PSIn In) : SV_TARGET
{
	float3 N = normalize(In.normal);
	float3 T = normalize(In.tangent);
	float3 n = (isNormalMap)* UnpackNormals(In.texCoord, N, T) +
		(1.0f - isNormalMap) * N;
	//n = (n + 1) / 2;
	const float gamma = 2.2;
	return 	pow(float4(n, 1), float4(gamma, gamma, gamma, 1));
}