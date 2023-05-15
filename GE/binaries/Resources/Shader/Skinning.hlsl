
#ifndef _SKINNING_H
#define _SKINNING_H

struct SkinningInfo
{
    float3 Pos;
    float3 Normal;
    float3 Tangent;
    float3 Binormal;
};

cbuffer ModelSkining
{
    int UseSkinning;
    float3 rootOffset;
};

#ifdef INSTANCED	
    StructuredBuffer<int> InstancedUseSkinning;
    StructuredBuffer<float3> InstancedRootOffset;
#endif

cbuffer InstanceingSkinning
{
    int InstancingBoneCount;    

};

StructuredBuffer<matrix> ComputedMatrix : register(t11);
StructuredBuffer<matrix> InstancingComputedMatrix : register(t12);


SkinningInfo Skinning(float3 Pos, float3 Normal, float3 Tangent, float4 BlendWeight, float4 BlendIndex)
{
    SkinningInfo Info = (SkinningInfo) 0;

    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;

        matrix matBone = ComputedMatrix[(int) BlendIndex[i]];                   
        
        
        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
        Info.Pos -= rootOffset * BlendWeight[i];        
            
        
        
        Info.Normal += (mul(float4(Normal, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Tangent += (mul(float4(Tangent, 0.f), matBone) * BlendWeight[i]).xyz;
    }

    Info.Normal = normalize(Info.Normal);
    Info.Tangent = normalize(Info.Tangent);

    return Info;
}

#ifdef INSTANCED
SkinningInfo Skinning_Instancing(float3 Pos, float3 Normal, float3 Tangent, float4 BlendWeight, float4 BlendIndex,int InstanceID)
{
    SkinningInfo Info = (SkinningInfo) 0;

    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;

        matrix matBone = InstancingComputedMatrix[InstanceID * InstancingBoneCount + (int) BlendIndex[i]];

        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
        Info.Pos -= InstancedRootOffset[InstanceID] * BlendWeight[i];  

        Info.Normal += (mul(float4(Normal, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Tangent += (mul(float4(Tangent, 0.f), matBone) * BlendWeight[i]).xyz;
    }

    Info.Normal = normalize(Info.Normal);
    Info.Tangent = normalize(Info.Tangent);

    return Info;
}
#endif


#endif