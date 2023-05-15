
#include "Skinning.hlsl"

struct ObjectMatrices
{
    matrix worldView;
    matrix normalViewMatrix;
    matrix worldViewProj;
};



cbuffer perModel
{
    ObjectMatrices ObjMatrices;
}

cbuffer frame
{
    matrix lightSpaceMat; 
};

struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float2 uv : TEXCOORD0;
    
    float4 blendweight : BLENDWEIGHT;
    float4 blendindex : BLENDINDEX;
#ifdef INSTANCED
	uint instanceID : SV_InstanceID;
#endif
};

struct PSIn
{
    float4 position : SV_POSITION;
    float3 viewPosition : POSITION0;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float2 uv : TEXCOORD1;
    
       
    float4 blendweight : BLENDWEIGHT;
    float4 blendindex : BLENDINDEX;

    
#ifdef INSTANCED
	uint instanceID			: SV_InstanceID;
#endif
};



PSIn VSMain(VSIn In)
{
    const float4 pos = float4(In.position, 1);

    PSIn Out = (PSIn) 0;
    
    SkinningInfo Info;
    Info.Pos = In.position;
    Info.Normal = In.normal;
    Info.Tangent = In.tangent;
    
#ifdef INSTANCED
    if (InstancedUseSkinning[In.instanceID] == 1)
        Info = Skinning_Instancing(In.position, In.normal, In.tangent, In.blendweight, In.blendindex , In.instanceID);
#else
    if (UseSkinning == 1)
        Info = Skinning(In.position, In.normal, In.tangent, In.blendweight, In.blendindex);
#endif

    
    
    
#ifdef INSTANCED
    Out.position = mul(InstancedObjMatrices[In.instanceID].worldViewProj, float4(Info.Pos, 1));
    Out.viewPosition = mul(InstancedObjMatrices[In.instanceID].worldView, float4(Info.Pos,1)).xyz;
    Out.viewNormal = normalize(mul(InstancedObjMatrices[In.instanceID].normalViewMatrix, float4(Info.Normal, 0))).rgb;
    Out.viewTangent = normalize(mul(InstancedObjMatrices[In.instanceID].normalViewMatrix, float4(Info.Tangent,0))).rgb;
	Out.instanceID	 = In.instanceID;
#else
	//Out.position	 = mul(ObjMatrices.worldViewProj, pos);
    float4 clipPos = mul(ObjMatrices.worldViewProj, float4(Info.Pos, 1));
    Out.viewPosition = mul(ObjMatrices.worldView, float4(Info.Pos, 1)).xyz;
    Out.viewNormal = normalize(mul(ObjMatrices.normalViewMatrix, float4(Info.Normal, 0))).rgb;
    Out.viewTangent = normalize(mul(ObjMatrices.normalViewMatrix, float4(Info.Tangent, 0))).rgb;	
    Out.position = clipPos;   
#endif
    
    Out.uv = In.uv;
    return Out;
}

