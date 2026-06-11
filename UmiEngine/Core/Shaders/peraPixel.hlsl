#include "peraHeader.hlsli"

float4 main(Output input) : SV_TARGET
{
    float4 col = tex.Sample(smp, input.uv);
    return col;

    //float Y = dot(col.rgb, float3(0.299, 0.587, 0.114));
    //return float4(Y, Y, Y, 1);

    //色の反転
    //return float4(float3(1.0f, 1.0f, 1.0f) - col.rgb, col.a);
    //return float4(1.0f - col.rgb, col.a);

    //return float4(col.rgb - fmod(col.rgb, 0.25f), col.a);
}