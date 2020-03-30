#include "Shared.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float4x4 world;
	float4x4 view;
	float4x4 proj;
}

// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
	matrix wvp = mul(proj, mul(view, world));
	output.position = mul(wvp, float4(input.position, 1.0f));
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;
	output.color = colorTint;
	output.normal = mul((float3x3)world, input.normal); //TODO: change to use inverse transpose
	output.uv = input.uv;
	return output;
}