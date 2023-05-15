#include "Skinning.hlsl"

struct ObjectMatrices
{
    matrix matWorld;
    matrix wvp;
};



cbuffer perModel
{
    ObjectMatrices ObjMatrices;
}

#ifdef INSTANCED
	StructuredBuffer<ObjectMatrices> InstancedObjMatrices;        
#endif

struct VSIn
{
	float3 position : POSITION;
    float3 viewPosition : POSITION0;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float2 uv : TEXCOORD1;
    float4 blendweight : BLENDWEIGHT;
    float4 blendindex : BLENDINDEX;

#ifdef INSTANCED
	uint instanceID : SV_InstanceID;
#endif
};

struct PSIn
{
	float4 position : SV_POSITION;
	
};

PSIn VSMain(VSIn In)
{
	PSIn Out;
    SkinningInfo Info;
    Info.Pos = In.position;
    Info.Normal = In.position;
    Info.Tangent = In.position;
    
#ifdef INSTANCED
	
    if (InstancedUseSkinning[In.instanceID] == 1)
        Info = Skinning_Instancing(In.position, In.position, In.position, In.blendweight, In.blendindex, In.instanceID);
    
    Out.position = mul(InstancedObjMatrices[In.instanceID].wvp, float4(Info.Pos, 1));
    
#else      
    
 

    if (UseSkinning == 1)
        Info = Skinning(In.position, In.position, In.position, In.blendweight, In.blendindex);

	
    Out.position = mul(ObjMatrices.wvp, float4(Info.Pos, 1));
    
#endif

	return Out;
}