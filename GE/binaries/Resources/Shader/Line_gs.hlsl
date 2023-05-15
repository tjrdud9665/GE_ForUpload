
//cbuffer perLine
//{
//	float3 p1;
//	float pad1;
//	float3 p2;
//	float pad2;
//};

struct lineInfo
{
    float3 p1;
    float3 p2;
    float3 color;
};

StructuredBuffer<lineInfo> lines;

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
[maxvertexcount(2)]
void GSMain(point GSIn gin[1], inout LineStream<PSIn> lineStream)
{
	PSIn Out[2];
    
    Out[0].position = mul(proj, mul(view, float4(lines[gin[0].instanceID].p1, 1)));
    Out[1].position = mul(proj, mul(view, float4(lines[gin[0].instanceID].p2, 1)));
    
    Out[0].color = lines[gin[0].instanceID].color;
    Out[1].color = lines[gin[0].instanceID].color;

	lineStream.RestartStrip();
	lineStream.Append(Out[0]);
	lineStream.Append(Out[1]);
}