
#include "Particle.hlsl"

cbuffer ParticleCBuffer 
{
    ParticleUpdateInfo particleInfo;       
    
    float TimeAcc;        
    float DeltaTime;    
    float2 __pad;
    
    float4 WorldOffset;
    
};


RWStructuredBuffer<ParticleInfo> ParticleArray : register(u0);
RWStructuredBuffer<ParticleInfoShared> ParticleShare : register(u1);

// 쓰레드 그룹당 쓰래드 개수
// 1024 가 최대
 /*
1024,1,1 과 와 32,32,1 는 갯수는 같지만 ThreadID 즉 넘버링에서 차이가 난다.

하나의 그룹에 쓰레드가 64 개
멀티 쓰래드로 동작하기 때문에 동기화를 해주어야한다.
*/
[numthreads(64, 1, 1)] // 스레드 그룹 스레드 수를 지정한다.
void CSMain(uint3 ThreadID : SV_DispatchThreadID)
{
    //Setup Share 
    ParticleShare[0].SpawnEnable = particleInfo.SpawnEnable; //SpawnRate
    
    ParticleShare[0].ScaleMin = particleInfo.ScaleMin;
    ParticleShare[0].ScaleMax = particleInfo.ScaleMax;
    ParticleShare[0].ColorMin = particleInfo.ColorMin;
    ParticleShare[0].ColorMax = particleInfo.ColorMax;
    
    ParticleShare[0].SpriteCount = particleInfo.SpriteCount;
    ParticleShare[0].SpriteCol = particleInfo.SpriteCol;
    ParticleShare[0].SpriteRow = particleInfo.SpriteRow;
    ParticleShare[0].WorldOffset = WorldOffset;

    	
    if (particleInfo.SpawnCountMax <= ThreadID.x)
        return;

    
	// 파티클이 살아있는 파티클인지 판단한다.
    if (ParticleArray[ThreadID.x].Alive == 0)
    {
        int SpawnEnable = ParticleShare[0].SpawnEnable;
        int Exchange = 0;
        
        if (ParticleShare[0].SpawnEnable != 0)
        {
            int InputValue = SpawnEnable - 1;

            //CAS
            InterlockedExchange(ParticleShare[0].SpawnEnable, InputValue, Exchange);
            
            if (Exchange == SpawnEnable)
                ParticleArray[ThreadID.x].Alive = 1;
        }

        if (ParticleArray[ThreadID.x].Alive == 0)
            return;

	
        
        
        //Make Randoms..
        float x = ((float) ThreadID.x / (float) particleInfo.SpawnCountMax) + TimeAcc;
        
        float3 randomHash = (1 + ThreadID.x) * float3(TimeAcc % 1024, TimeAcc / 1024, TimeAcc);
        
        float hash = FracRandom(randomHash.xy);
        
        float3 RandomPos = float3(FracRandom(randomHash.yx), FracRandom(randomHash.zy), FracRandom(randomHash.zx));
        
        float3 StartRange = particleInfo.StartMax - particleInfo.StartMin;
                                
        ParticleArray[ThreadID.x].WorldPos = RandomPos * StartRange + particleInfo.StartMin + WorldOffset.xyz;
                                                            
        

        ParticleArray[ThreadID.x].LifeTime = 0.f;
        ParticleArray[ThreadID.x].LifeTimeMax = hash * (particleInfo.LifeTimeMax - particleInfo.LifeTimeMin) + particleInfo.LifeTimeMin;
        
        //g_ParticleArray[ThreadID.x].SpriteIndex = 0;
        
        //-1~~1        
        float3 Dir = float3(SimplexNoise(randomHash.xyz), SimplexNoise(randomHash.yxz), SimplexNoise(randomHash.xzy));        
        
        
        float3 minDir = (particleInfo.MoveDirMin);
        float3 maxDir = (particleInfo.MoveDirMax);
        
        ParticleArray[ThreadID.x].Speed = hash * (particleInfo.SpeedMax - particleInfo.SpeedMin) + particleInfo.SpeedMin;
        //g_ParticleArray[ThreadID.x].Dir = Dir;
        ParticleArray[ThreadID.x].Dir.x = clamp(Dir.x, minDir.x, maxDir.x);        
        ParticleArray[ThreadID.x].Dir.y = clamp(Dir.y, minDir.y, maxDir.y);            
        ParticleArray[ThreadID.x].Dir.z = clamp(Dir.z, minDir.z, maxDir.z);
        
        ParticleArray[ThreadID.x].Dir = normalize(ParticleArray[ThreadID.x].Dir);
            
        
        ParticleArray[ThreadID.x].SpriteIdx = 0;

    }
	// 현재 생성이 되어 있는 파티클일 경우
    else
    {
        ParticleArray[ThreadID.x].LifeTime += DeltaTime;

        float3 MovePos = (float3) 0.f;

        MovePos = ParticleArray[ThreadID.x].Dir * ParticleArray[ThreadID.x].Speed * DeltaTime;

        ParticleArray[ThreadID.x].WorldPos += MovePos;
        
        float ratio = ParticleArray[ThreadID.x].LifeTime / ParticleArray[ThreadID.x].LifeTimeMax;
        //ratio로 SpriteIndex
        ParticleArray[ThreadID.x].SpriteIdx = particleInfo.SpriteCount * ratio-1;
        
            

        if (ParticleArray[ThreadID.x].LifeTime >= ParticleArray[ThreadID.x].LifeTimeMax || particleInfo.KillAllParticle)
        {
            ParticleArray[ThreadID.x].WorldPos = WorldOffset.xyz;
            ParticleArray[ThreadID.x].Alive = 0;
        }

    }
}