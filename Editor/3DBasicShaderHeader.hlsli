struct Output
{
    float4 svpos : SV_POSITION;
    float4 pos : POSITION;
};

SamplerState smp : register(S0);

cbuffer cbuff0 : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
//    float3 eye;
}

struct BasicType
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //UV値
};
