
cbuffer perModel
{
    matrix worldViewProj;
}

struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float2 texCoord : TEXCOORD0;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSIn VSMain(VSIn In)
{
    PSIn Out;
    Out.position = mul(worldViewProj, float4(In.position, 1));
    Out.uv = In.texCoord;
    return Out;
}