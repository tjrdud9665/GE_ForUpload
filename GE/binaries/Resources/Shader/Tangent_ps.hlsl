

struct PSIn
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD4;
};

float4 PSMain(PSIn In) : SV_TARGET
{
    float3 T = normalize(In.tangent);
	T = (T + 1) / 2;
	return 	float4(T, 1);
}