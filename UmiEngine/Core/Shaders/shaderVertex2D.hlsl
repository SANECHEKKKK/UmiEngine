float4x4 mtx;

float4 main(in float4 position : POSITION0,
			in float2 texcoord : TEXCOORD0,
			out float2 outTexcoord : TEXCOORD0) : SV_Position
{
    outTexcoord = texcoord;
	return mul(position, mtx);
}