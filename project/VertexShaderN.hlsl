#ifndef __GGP_VERTEX_STRUCTS__
#define __GGP_VERTEX_STRUCRS__
#include "VertStructs.hlsli"
#endif

#ifndef __GGP_VERT_TO_PIXELN__
#define __GGP_VERT_TO_PIXELN__
#include "VertToPixStructN.hlsli"
#endif

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	matrix world;
	matrix view;
	matrix projection;
	matrix worldInvTrans;
	float3 cameraPos;
	matrix viewInv;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixelN main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixelN output;

	// Here we're essentially passing the input position directly through to the next
	// stage (rasterizer), though it needs to be a 4-component vector now.  
	// - To be considered within the bounds of the screen, the X and Y components 
	//   must be between -1 and 1.  
	// - The Z component must be between 0 and 1.  
	// - Each of these components is then automatically divided by the W component, 
	//   which we're leaving at 1.0 for now (this is more useful when dealing with 
	//   a perspective projection matrix, which we'll get to in future assignments).
	matrix wvp = mul(projection, mul(view, world));
	output.position = mul(wvp, float4(input.position, 1.0f));
	float4 worldPos = mul(world, float4(input.position, 1));
	output.worldPos = float3(worldPos.x, worldPos.y, worldPos.z);

	output.cameraPos = cameraPos;
	//output.cameraPos = float3(viewInv._14, viewInv._24, viewInv._34);
	output.uv = input.uv;

	// Pass the color through 
	// - The values will be interpolated per-pixel by the rasterizer
	// - We don't need to alter it here, but we do need to send it to the pixel shader
	output.color = colorTint;

	output.normal = mul((float3x3)worldInvTrans, input.normal);
	output.tangent = mul((float3x3)worldInvTrans, input.tangent);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}