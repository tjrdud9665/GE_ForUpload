
#ifndef _PARTICLE_H
#define _PARTICLE_H





struct ParticleInfo
{
    float3 WorldPos;
    float Speed;
    
    float3 Dir;
    float LifeTime;
    
    float LifeTimeMax;
    int Alive;
    
    int SpriteIdx;
    float __pad;
    
    //int SpriteIndex; //현재인덱스인데 -> LifeTime에 의한 Ratio로 쓸건지?
};


struct ParticleInfoShared
{
    uint SpawnEnable;
    float3 ScaleMin;
    
    int SpriteCount;    //Colum x Row  /> Min Max 1~~n

    float3 ScaleMax;

    float4 ColorMin;
    float4 ColorMax;    
    
    int SpriteRow;
    int SpriteCol;    
    float2 __Pad;
    
    float4 WorldOffset;
    //Colum, Row 로 Uv를 gs 에서 조정
   
    
};

struct ParticleUpdateInfo
{
    uint SpawnEnable; // 파티클 생성여부
    float3 StartMin; // 파티클이 생성될 영역의 Min
    
    float3 StartMax; // 파티클이 생성될 영역의 Max    
    uint SpawnCountMax; // 생성될 파티클의 최대
    
    float3 ScaleMin; // 생성될 파티클 크기의 Min
    float LifeTimeMin; // 생성될 파티클이 살아있을 최소시간
    
    float3 ScaleMax; // 생성될 파티클 크기의 Max
    float LifeTimeMax; // 생성될 파티클이 살아있을 최대시간
    
    
    float4 ColorMin; // 생성될 파티클의 색상 Min    
    float4 ColorMax; // 생성될 파티클의 색상 Max    

    float SpeedMin; // 파티클의 최소 이동속도
    float3 MoveDirMin;
    
    float SpeedMax; // 파티클의 최대 이동속도
    float3 MoveDirMax;
    
    
    //ParticleSpriteInfo g_SpriteInfo[10]; //
    
    
    int SpriteCount;
    int KillAllParticle;
    int SpriteCol;  //Count
    int SpriteRow;
    //float2 __Pad;
};

float FracRandom(float2 coordinate)
{
    return frac(sin(dot(coordinate.xy, float2(12.9898, 78.233))) * 43758.5453);
}


float3 Mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 Mod289(float4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 Permute(float4 x)
{
    return Mod289(((x * 34.0) + 1.0) * x);
}

float4 TaylorInverseSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float SimplexNoise(float3 v)
{
    const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);
    const float4 D = float4(0.0, 0.5, 1.0, 2.0);

  // NOTE: First corner
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);

  // NOTE: Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy; // 2.0 * C.x = 1 / 3 = C.y
    float3 x3 = x0 - D.yyy; // -1.0 + 3.0 * C.x = -0.5 = -D.y

  // NOTE: Permutations
    i = Mod289(i);
    float4 p = Permute(Permute(Permute(
             i.z + float4(0.0, i1.z, i2.z, 1.0))
           + i.y + float4(0.0, i1.y, i2.y, 1.0))
           + i.x + float4(0.0, i1.x, i2.x, 1.0));

  // NOTE: Gradients: 7x7 points over a square, mapped onto an octahedron.
  // NOTE: The ring size 17 * 17 = 289 is close to a multiple of 49 (49 * 6 = 294)
    float n_ = 0.142857142857; // 1.0 / 7.0
    float3 ns = n_ * D.wyz - D.xzx;

    float4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)

    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0 * x_); // mod(j,N)

    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - abs(x) - abs(y);

    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);
  
    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, float4(0.0, 0.0, 0.0, 0.0));

    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);

  // NOTE: Normalize gradients
    float4 norm = TaylorInverseSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

  // Mix final noise value
    float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m * m, float4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}


#endif
