

struct PSIn
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

float4 PSMain(PSIn In) : SV_TARGET
{
	return float4(In.color, 1);
}