struct Output
{
    float4 svpos : SV_POSITION;
    float4 pos : POSITION;
    float4 normal : NORMAL0;
    float4 vnormal : NORMAL1;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};

Texture2D<float4> textur : register(t0);
Texture2D<float4> tex : register(t1);
Texture2D<float4> sph : register(t2);
Texture2D<float4> spa : register(t3);
Texture2D<float4> toon : register(t4);

SamplerState smp : register(S0);
SamplerState smpToon : register(S1);

cbuffer cbuff0 : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float3 eye;
}

cbuffer Material : register(b1)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
}