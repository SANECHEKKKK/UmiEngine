cbuffer SceneMatrix : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

float4 main(float3 pos : POSITION) : SV_POSITION
{
    float4 p = float4(pos, 1.0f);
    p = mul(world, p);
    return mul(mul(proj, view), p);
}