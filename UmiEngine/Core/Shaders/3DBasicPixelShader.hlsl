#include "3DBasicShaderHeader.hlsli"

float4 BasicPS(Output input) : SV_TARGET
{
    float4 albedo = mat.albedoIndex != 0xFFFFFFFF
    ? textures[mat.albedoIndex].Sample(smp, input.uv) * mat.baseColor
    : mat.baseColor;
    clip(albedo.a - 0.5);
    return albedo;
}