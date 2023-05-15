
struct PSIn
{
    float4 postion : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 Color : COLOR0;
    
    uint InstanceID : SV_InstanceID;
};


//vs 0 9
//gs 11 19
//ps21  29

cbuffer cbParticleSurface : register(b12)
{
    float hasTex;    //1,0
 
};

Texture2D texDiffuseMap;
SamplerState sLinearSampler;

float4 PSMain(PSIn In) : SV_TARGET
{    
    float2 uv = In.uv;
    
    float4 outColor = hasTex * texDiffuseMap.Sample(sLinearSampler, uv) * float4(In.Color)
                    + (1.0f - hasTex) * float4(In.Color);
    
    if (outColor.a <= 0.0f)
    {
        clip(-1);

    }
    return outColor;      
    
}