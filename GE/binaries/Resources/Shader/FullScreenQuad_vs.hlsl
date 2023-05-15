//https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-struct


struct VSIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct PSIn
{
    centroid float4 position : SV_POSITION;
    centroid noperspective
    float2 uv : TEXCOORD0;
};

PSIn VSMain(VSIn In)
{
    PSIn Out;
    Out.position = float4(In.position, 1);
    Out.uv = In.uv;
    return Out;
}