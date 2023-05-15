

struct PSIn
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD4;
};

float4 PSMain(PSIn In) : SV_TARGET
{
	float3 N = normalize(In.normal);
	float3 T = normalize(In.tangent - dot(N, In.tangent) * N);
	float3 B = normalize(cross(N, T));
	return 	float4((B+1)/2, 1);
}