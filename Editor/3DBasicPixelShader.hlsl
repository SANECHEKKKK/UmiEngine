#include "3DBasicShaderHeader.hlsli"

float4 BasicPS(Output input) : SV_TARGET
{
    return float4(0, 0, 0, 1); //UVをそのまま出力してみる)
}