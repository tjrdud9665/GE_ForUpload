

struct PHONG_Surface
{
    float3 N;
    float3 diffuseColor;
    float3 specularColor;
    float shininess;
};



float3 Phong(PHONG_Surface s, float3 L, float3 V, float3 lightColor)
{
    const float3 N = s.N;
    const float3 R = normalize(2 * N * dot(N, L) - L);
    float diffuse = max(0.0f, dot(N, L));

    float3 Id = lightColor * s.diffuseColor * diffuse;

#ifdef BLINN_PHONG
	const float3 H = normalize(L + V);
	float3 Is = lightColor * s.specularColor * pow(max(dot(N, H), 0.0f), 4.0f * s.shininess) * diffuse;
#else
    float3 Is = lightColor * s.specularColor * pow(max(dot(R, V), 0.0f), s.shininess) * diffuse;
#endif
	
    return Id + Is;
}