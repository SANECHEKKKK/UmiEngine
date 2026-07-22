#include "3DBasicShaderHeader.hlsli"

cbuffer BoneBuffer : register(b2)
{
    matrix boneTransforms[128];
};

Output SkinnedVS(
    float4 pos         : POSITION,
    float4 normal      : NORMAL,
    float2 uv          : TEXCOORD,
    int4   boneIDs     : BLENDINDICES,
    float4 boneWeights : BLENDWEIGHT)
{
    Output output;

    float weightSum = boneWeights.x + boneWeights.y +
                      boneWeights.z + boneWeights.w;

    float4 skinned = float4(0, 0, 0, 0);

    if (weightSum > 0.0001f)
    {
        float4 w = boneWeights / weightSum;

        [unroll]
        for (int i = 0; i < 4; ++i)
        {
            int b = boneIDs[i];
            if (b < 0 || b >= 128)
                continue;

            skinned += mul(boneTransforms[b], float4(pos.xyz, 1.0f)) * w[i];
        }
        skinned.w = 1.0f;
    }
    else
    {
        skinned = float4(pos.xyz, 1.0f);
    }

    float4 worldPos = mul(world, skinned);
    output.svpos = mul(mul(proj, view), worldPos);
    output.pos   = worldPos;
    output.uv    = uv;
    return output;
}
