#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	float3 color;
	matrix world;
	matrix view;
	matrix proj;
}

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;       // XYZ Normal
	float3 tangent		: TANGENT;
	float2 uv			: TEXCOORD;		// XY  UV
	
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{

	// Set up output struct
	VertexToPixel output;

	// Modifying the position using the provided transformation (world) matrix
	matrix wvp = mul(proj, mul(view, world));
	output.position = mul(wvp, float4(input.position, 1.0f));

	// Calculate the final world position of the vertex
	output.worldPos = mul(world, float4(input.position, 1.0f)).xyz;

	// Modify the normal so its also in world space
	output.normal = mul((float3x3)world, input.normal);
	output.normal = normalize(output.normal);

	// Modify the tangent much like the normal
	output.tangent = mul((float3x3)world, input.tangent);
	output.tangent = normalize(output.tangent);

	// Tints the color before passing it through
	output.color = float4(color,1);

	output.uv = input.uv;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}