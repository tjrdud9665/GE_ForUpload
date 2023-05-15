
struct SolidInfo
{
    float3 p1;
    float3 p2;
    float3 p3;
    float3 color;
};

StructuredBuffer<SolidInfo> solids;

cbuffer renderConsts
{
    matrix view;
    matrix proj;
};

struct GSIn
{
    uint instanceID : SV_InstanceID;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

// Note about 'maxvertexcount(N)'
// [NVIDIA08]:	peak perf when GS outputs 1-20 scalars
//				50% perf if 27-40 scalars are output
//				where scalar count = N * PSIn(scalarCount)

[maxvertexcount(3)]
void GSMain(point GSIn gin[1], inout TriangleStream<PSIn> outStream)
{
    PSIn Out[3];
    
    Out[0].position = mul(proj, mul(view, float4(solids[gin[0].instanceID].p1, 1)));
    Out[1].position = mul(proj, mul(view, float4(solids[gin[0].instanceID].p2, 1)));
    Out[2].position = mul(proj, mul(view, float4(solids[gin[0].instanceID].p3, 1)));
    
    Out[0].color = solids[gin[0].instanceID].color;
    Out[1].color = solids[gin[0].instanceID].color;
    Out[2].color = solids[gin[0].instanceID].color;

    
    outStream.Append(Out[0]);
    outStream.Append(Out[1]);
    outStream.Append(Out[2]);
    outStream.RestartStrip();
}