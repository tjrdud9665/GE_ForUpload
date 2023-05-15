
#include "Skinning.hlsl"


struct ObjectMatrices
{
	matrix matWorld;
    matrix wvp;
};



cbuffer perModel
{
    ObjectMatrices ObjMatrices; //Matrix *3           
};


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
	float4 svPosition     : SV_POSITION;
	float4 worldPosition  : POSITION0;	
   
	
#ifdef INSTANCED
	uint instanceID			: SV_InstanceID;
#endif
};


PSIn VSMain(VSIn In)
{
	PSIn vsOut;	
	   
    SkinningInfo Info;
    Info.Pos = In.position;    
    
#ifdef INSTANCED
    if (InstancedUseSkinning[In.instanceID] == 1)
        Info = Skinning_Instancing(In.position, float3(0, 0, 0), float3(0, 0, 0), In.blendweight, In.blendindex , In.instanceID);
#else
    if (UseSkinning == 1)
        Info = Skinning(In.position, float3(0, 0, 0), float3(0, 0, 0), In.blendweight, In.blendindex);
#endif
    
    
#ifdef INSTANCED
	const matrix mWorld = InstancedObjMatrices[In.instanceID].matWorld;
	const matrix mWVP = InstancedObjMatrices[In.instanceID].wvp;
#else
    const matrix mWorld = ObjMatrices.matWorld;
    const matrix mWVP = ObjMatrices.wvp;
#endif

    const float4 vPos = float4(Info.Pos, 1.0f);

	vsOut.worldPosition = mul(mWorld, vPos);
	vsOut.svPosition    = mul(mWVP  , vPos);

	return vsOut;
}