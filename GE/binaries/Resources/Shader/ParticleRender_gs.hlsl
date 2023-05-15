
#include "Particle.hlsl"

StructuredBuffer<ParticleInfo> ComputedParticleArray : register(t11);
StructuredBuffer<ParticleInfoShared> ComputedParticleShare : register(t12);



void MatrixFindLookAt(in float3 QuadPos
    , in float4 CameraPostion
    , out matrix _outMat)
{
    matrix MScaling = (matrix) 0.f;
    MScaling._11_22_33 = float3(1, 1, 1);
    MScaling._44 = 1.f;

    

    
    
    float3 toCam = normalize(QuadPos - CameraPostion.xyz);
    //aTo B -> B-a
    float3 zAxis = toCam;
    float3 xAxis = normalize(cross(float3(0, 1, 0), zAxis));
    float3 yAxis = normalize(cross(zAxis, xAxis));   
	
    matrix MRotation = matrix(xAxis.x, yAxis.x, zAxis.x, 0,
						      xAxis.y, yAxis.y, zAxis.y, 0,
						      xAxis.z, yAxis.z, zAxis.z, 0,
						        0, 0, 0, 1.0f);        
   
    
    _outMat = MRotation;
}



//Quad가 카메라를 바라볼수 있도록 유도

cbuffer perModel : register(b11)
{     
    matrix ViewProj;    
    
    float4 CameraPostion;//LookAt을 구성 
    
   
};

struct GSIn
{
    float3 viewPosition : POSITION0;
    float2 uv : TEXCOORD0;

    uint instanceID : SV_InstanceID;
};

struct PSIn
{
    float4 postion : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 Color : COLOR0;    
    
    uint InstanceID : SV_InstanceID;
};

static float3 BillboardQudLocalPos[4] =
{
    { -0.5f, -0.5f, 0.f },
    { -0.5f, 0.5f, 0.f },
    { 0.5f, 0.5f, 0.f },
    { 0.5f, -0.5f, 0.f }
};

//1     2
//
//0     3


[maxvertexcount(6)]
void GSMain(point GSIn gin[1], inout TriangleStream<PSIn> outputStream)
{
    	//사각형
    PSIn output[4] =
    {
        (PSIn) 0.f, (PSIn) 0.f, (PSIn) 0.f, (PSIn) 0.f
    };
    GSIn vtx = gin[0];
    
    uint id = (uint) vtx.instanceID;
    
    //디젔다면
    if (ComputedParticleArray[id].Alive == 0)    
        return;
    
    float Ratio = ComputedParticleArray[id].LifeTime / ComputedParticleArray[id].LifeTimeMax;
    
    float3 Scale = lerp(ComputedParticleShare[0].ScaleMin, ComputedParticleShare[0].ScaleMax, float3(Ratio, Ratio, Ratio));
    //float3(50, 50, 50);    
    
    float4 Color = lerp(ComputedParticleShare[0].ColorMin, ComputedParticleShare[0].ColorMax, float4(Ratio, Ratio, Ratio, Ratio));
    
    
    int spritCount = ComputedParticleShare[0].SpriteCount;
    float spriteCol = (float)ComputedParticleShare[0].SpriteCol;             //Never 0
    float spriteRow = (float)ComputedParticleShare[0].SpriteRow;
    
    int curCol = ComputedParticleArray[id].SpriteIdx % spriteCol + 1;
    int curRow = ComputedParticleArray[id].SpriteIdx / spriteCol + 1;
    //2 /1 ( col / Row)
    //  
    output[0].uv = float2((curCol - 1) / spriteCol, (curRow) / spriteRow);
    output[1].uv = float2((curCol - 1) / spriteCol, (curRow - 1) / spriteRow);    
    
    output[2].uv = float2((curCol) / spriteCol, (curRow - 1) / spriteRow);
    output[3].uv = float2((curCol) / spriteCol, (curRow ) / spriteRow);
    //1     2
    //
    //0     3
    
    //output[0].uv = float2(0.f, 1.f);
    //output[1].uv = float2(0.f, 0.f);
    //output[2].uv = float2(1.f, 0.f);
    //output[3].uv = float2(1.f, 1.f);
    

    //+ WorldOffset.xyz
    matrix look;
    MatrixFindLookAt(ComputedParticleArray[id].WorldPos, CameraPostion, look);
    
    
	// 4개의 최종 정점정보를 만들어준다.
    for (int i = 0; i < 4; ++i)
    {
        
        float3 WorldPos = ComputedParticleArray[id].WorldPos;
        WorldPos += mul(look, float4(BillboardQudLocalPos[i] * Scale, 1));

        
        output[i].postion = mul(ViewProj, float4(WorldPos, 1));
        output[i].Color = Color;
        

    }

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}