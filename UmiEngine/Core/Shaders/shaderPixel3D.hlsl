cbuffer MaterialBuffer : register(b1)
{
    float4 materialColor;
    bool hasTexture;
    float3 padding;
};

Texture2D objTexture : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 textureColor;
    
    if (hasTexture)
    {
        textureColor = objTexture.Sample(samplerState, input.texcoord);
    }
    else
    {
        textureColor = materialColor;
    }
    
    float3 lightDir = normalize(float3(0.2, 0.2, -0.2));
    float diffuse = max(dot(normalize(input.normal), lightDir), 0.0); // 0.2 = ambient
    
    

    return float4(textureColor.rgb * diffuse, textureColor.a);
    
    //float4 textureColor = diffuseMap.Sample(samplerState, input.texcoord);
    //return textureColor;
}