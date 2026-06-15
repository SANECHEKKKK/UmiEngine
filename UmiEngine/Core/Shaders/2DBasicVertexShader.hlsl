#include "2DBasicShaderHeader.hlsli"

BasicType BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    BasicType output; //ピクセルシェーダへ渡す値
    pos = mul(world, pos);
    output.svpos = mul(mul(proj, view), pos); //シェーダでは列優先なので注意
    output.uv = uv;
    return output;
}