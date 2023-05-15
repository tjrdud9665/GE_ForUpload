

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PSMain(PSIn In) : SV_TARGET
{
    const float4 outColor = float4(In.uv, 0, 1);
    return outColor;
}