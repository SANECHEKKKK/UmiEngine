cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer BoneBuffer : register(b2)
{
    matrix boneTransforms[100];
};

struct VSInput
{
    float3 position : POSITION;
    float3 Normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    int4 boneIDs : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

PSInput main(VSInput input)
{
    PSInput output;
	
    float4 totalPosition = float4(0, 0, 0, 0);
    float4 totalNormal = float4(0, 0, 0, 0);

    for (int i = 0; i < 4; i++)
    {
        if (input.boneWeights[i] > 0.0f)
        {
            float4 localPosition = mul(float4(input.position, 1.0f), boneTransforms[input.boneIDs[i]]);
            totalPosition += localPosition * input.boneWeights[i];
			
            float4 localNormal = mul(float4(input.Normal, 0.0f), boneTransforms[input.boneIDs[i]]);
            totalNormal += localNormal * input.boneWeights[i];
        }
    }
	
    if (totalPosition.w == 0.0f)
    {
        totalPosition = float4(input.position, 1.0f);
        totalNormal = float4(input.Normal, 0.0f);
    }
	
    totalPosition = mul(totalPosition, world);
    totalPosition = mul(totalPosition, view);
    totalPosition = mul(totalPosition, projection);
	
    output.position = totalPosition;
    output.texcoord = input.texcoord;
    output.Normal = normalize(mul(totalNormal.xyz, (float3x3) world));

	//float4 worldPos = mul(float4(input.position, 1.0f), world);
	//float4 viewPos = mul(worldPos, view);
	//output.position = mul(viewPos, projection);
	//
	//output.texcoord = input.texcoord;
	//output.Normal = mul(input.Normal, (float3x3) world);
	
	//float4 totalPosition = float4(0, 0, 0, 0);
	//for (int i = 0; i < 4; i++)
	//{
	//    if (input.boneWeights[i] > 0)
	//    {
	//        float4 localPosition = mul(float4(input.position, 1.0f),
	//                               boneTransforms[input.boneIDs[i]]);
	//        totalPosition += localPosition * input.boneWeights[i];
	//   }
	//}
	
    return output;
}