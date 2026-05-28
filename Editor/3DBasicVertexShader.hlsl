#include "3DBasicShaderHeader.hlsli"

Output BasicVS(
float4 pos : POSITION,
float4 normal : NORMAL,
float2 uv : TEXCOORD)
{
    Output output; //ピクセルシェーダへ渡す値
    pos = mul(world, pos);
    output.svpos = mul(mul(proj, view), pos); //シェーダでは列優先なので注意
    output.pos = pos;
    return output;
}