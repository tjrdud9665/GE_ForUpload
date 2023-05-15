
struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

struct textureInfo
{
    float3 diffuse;
    float alpha;
    
    float isDiffuseMap;
    float3 pad;
};

cbuffer perObject : register(b11)
{
    textureInfo texInfo;
   
};

Texture2D texDiffuseMap;

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;
    AddressU = WRAP;
    AddressV = WRAP;
};

float4 PSMain(PSIn In) : SV_TARGET
{
	float2 uv = In.texCoord;    
    
    float4 outColor = texInfo.isDiffuseMap * texDiffuseMap.Sample(samAnisotropic, uv) * float4(texInfo.diffuse, texInfo.alpha)
                    + (1.0f - texInfo.isDiffuseMap) * float4(texInfo.diffuse, texInfo.alpha);
    if (outColor.a <= 0.0f)
    {
        clip(-1);

    }    
     return outColor;
}