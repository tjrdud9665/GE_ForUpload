// source: http://richardssoftware.net/Home/Post/25

struct PSIn
{
	float4 HPos : SV_POSITION;
	float3 LPos : POSITION;
};

TextureCube texSkybox;
SamplerState samWrap;

float4 PSMain(PSIn In) : SV_TARGET
{
    return texSkybox.SampleLevel(samWrap, In.LPos, 0);
    //return pow(texSkybox.SampleLevel(samWrap, In.LPos, 0), 2.2f);

}
