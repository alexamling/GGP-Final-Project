#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	DirectionalLight dirLight;
	PointLight pntLight;
	float3 AmbientColor;
	float Specularity;
	float3 cameraPosition;
}

// Texture-related resources
Texture2D diffuseTexture	: register(t0);
Texture2D normalMap			: register(t1);
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
	// Sample the texture to get a color and tint
	float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv).rgb;
	// surfaceColor *= input.color.rgb;

	// Grab the normal map sample and UNPACK THE NORMAL
	float3 normalFromMap = normalMap.Sample(samplerOptions, input.uv).rgb * 2 - 1;

	// Simplify this code as you see fit
	float3 T = input.tangent; // Must be normalized
	T = normalize(T - input.normal * dot(T, input.normal));
	// Gram-Schmidt orthogonalization
	float3 B = cross(T, input.normal);
	float3x3 TBN = float3x3(T, B, input.normal);

	// Adjust and overwrite the existing normal
	input.normal = normalize(mul(normalFromMap, TBN));

	// DIRECTIONAL LIGHTS ---------------------------

	// DL 1
	float3 FinalDLColor = computeColor(input.normal, dirLight.Direction, input.worldPos, 
		dirLight.DiffuseColor, cameraPosition, surfaceColor, dirLight.Intensity, Specularity);

	// POINT LIGHT ---------------------------------
	float3 pl_Direction = normalize(input.worldPos - pntLight.Position);
	float3 FinalPLColor = computeColor(input.normal, pl_Direction, input.worldPos, pntLight.Color, 
		cameraPosition, surfaceColor, pntLight.Intensity, Specularity);
	FinalPLColor *= computeAttentuation(pntLight.Position, input.worldPos, pntLight.Range);

	// FINAL LIGHT ---------------------------------
	float3 finalLight = AmbientColor + FinalDLColor + FinalPLColor;

	return float4(finalLight , 1);
}