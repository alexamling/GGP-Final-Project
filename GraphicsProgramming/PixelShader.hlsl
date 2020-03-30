#include "Shared.hlsli"

cbuffer LightData : register(b0) 
{
	float3 camPos;
	float specExponent;
	DirectionalLight directionalLight1;
	DirectionalLight directionalLight2;
	DirectionalLight directionalLight3;
	PointLight pointLight1;
	PointLight pointLight2;
}

Texture2D baseTexture : register(t0);
SamplerState samplerOptions : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	float4 finalColor = float4(0,0,0,1);
	float3 surfaceColor = baseTexture.Sample(samplerOptions, input.uv).rgb;

	finalColor.rgb += CalculateLighting_Dir(surfaceColor, input.worldPos, input.normal, directionalLight1, camPos, specExponent);
	finalColor.rgb += CalculateLighting_Dir(surfaceColor, input.worldPos, input.normal, directionalLight2, camPos, specExponent);
	finalColor.rgb += CalculateLighting_Dir(surfaceColor, input.worldPos, input.normal, directionalLight3, camPos, specExponent);
	finalColor.rgb += CalculateLighting_Point(surfaceColor, input.worldPos, input.normal, pointLight1, camPos, specExponent);
	finalColor.rgb += CalculateLighting_Point(surfaceColor, input.worldPos, input.normal, pointLight2, camPos, specExponent);
	return finalColor;
}