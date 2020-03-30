#include "Shared.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float4x4 world;
	float4x4 view;
	float4x4 proj;
}

// --------------------------------------------------------
VertexToPixel_Normals main( VertexShaderInput input )
{
	VertexToPixel_Normals output;
	matrix wvp = mul(proj, mul(view, world));
	output.position = mul(wvp, float4(input.position, 1.0f));
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;
	output.color = colorTint;
	output.normal = mul((float3x3)world, input.normal); //TODO: change to use inverse transpose
	output.tangent = mul((float3x3)world, input.tangent);
	output.uv = input.uv;
	return output;
}