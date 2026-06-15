struct Output
{
    float4 svpos : SV_POSITION;
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

Texture2D textures[] : register(t0);
SamplerState smp : register(S0);

cbuffer cbuff0 : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

struct MaterialData
{
    // Texture indices
    uint albedoIndex;
    uint normalIndex;
    uint roughnessIndex;
    uint metallicIndex;

    // Scalar properties
    float4 baseColor;
    float roughness; // overridden by roughness map if present
    float metallic; // overridden by metallic map if present
    float emissive;
    float opacity;
};
ConstantBuffer<MaterialData> mat : register(b1);

struct BasicType
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //UV値
};