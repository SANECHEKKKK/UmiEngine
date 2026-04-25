cbuffer AlphaBuffer : register(b0)
{
    float alpha;
};

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(in float4 position : SV_Position,
            in float2 texcoord : TEXCOORD0) : SV_TARGET
{
    float4 color = tex.Sample(samplerState, texcoord);
    color.a *= alpha; // Modulate alpha
    return color;
}