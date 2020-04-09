#ifndef __GGP_PIXEL_STRUCTS__
#define __GGP_PIXEL_STRUCTS__
#include "PixelStructs.hlsli"
#endif

#ifndef __GGP_VERT_TO_PIXELN__
#define __GGP_VERT_TO_PIXELN__
#include "VertToPixStructN.hlsli"
#endif

Texture2D diffuseTexture	: register(t0);
Texture2D normalMap			: register(t1);
SamplerState samplerOptions	: register(s0);

cbuffer ExternalData : register(b0)
{
	Light l1;		// HOW DO I PASS IN ARRAYS? OR LOOP?
	Light l2;
	Light l3;
	Light l4;
	float specInt;
};

// calculate specular amount
float GetSpecular(float3 normal, float3 toLight, float3 toCamera, float specInt) {
	if (specInt > 0.f) {
		float3 midway = normalize(toLight + toCamera);
		return pow(saturate(dot(normal, midway)), 256 * specInt);
	}
	return 0.f;
}

// get directional light amount
float3 GetDirectionalLight(Light dl, float3 normal, float3 color, float3 worldPos, float3 toCamera, float specInt) {
	// get base amount
	float3 toLight = normalize(-dl.direction);
	float lightAmount = saturate(dot(normal, toLight));
	float3 baseColor = lightAmount * dl.diffuseColor * color + dl.ambientColor * color;
	return GetSpecular(normal, toLight, toCamera, specInt * any(lightAmount)) * baseColor + baseColor;
	//return float3(0, 0, 0);

}

// get point light amount
float3 GetPointLight(Light pl, float3 normal, float3 color, float3 worldPos, float3 toCamera, float specInt) {
	// get base amount
	float3 lightDir = normalize(worldPos - pl.direction);
	float3 lightAmount = saturate(dot(normal, -lightDir));

	// calculate fall off
	float dist = distance(worldPos, pl.direction);
	float att = saturate(1.0f - (dist * dist / (pl.radius * pl.radius)));
	float3 baseColor = att * lightAmount * pl.diffuseColor * color;
	return GetSpecular(normal, -lightDir, toCamera, specInt * any(lightAmount)) * baseColor + baseColor + pl.ambientColor * color;
	//return float3(0, 0, 0);
}

// in place of if statement for every light
float3 EvaluateLight(Light l, float3 normal, float3 color, float3 worldPos, float3 toCamera, float specInt) {
	if (l.type == 0) {
		return GetDirectionalLight(l, normal, color, worldPos, toCamera, specInt);
	}
	else if (l.type == 1)
	{
		return GetPointLight(l, normal, color, worldPos, toCamera, specInt);
	}

	// Whoops, Something is wrong (or not implemented yet)
	return float3(0, 0, 0);
}

float3 GetMappedNormal(float3 normal, float3 tangent, float2 uv) {
	// get normal from map
	float3 mapNormal = normalMap.Sample(samplerOptions, uv).rgb * 2 - 1;

	// Gram-Schmidt orthogonalization
	tangent = normalize(tangent - normal * dot(tangent, normal));

	// calculate biTangent
	float3 biTangent = cross(tangent, normal);

	// create change of basis matrix
	float3x3 cbMat = float3x3(tangent, biTangent, normal);

	// return adjusted normal
	return mul(mapNormal, cbMat);
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixelN input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	input.normal = GetMappedNormal(input.normal, input.tangent, input.uv);

	input.cameraPos = normalize(input.cameraPos - input.worldPos);
	input.color = (diffuseTexture.Sample(samplerOptions, input.uv).rgba + input.color) * .5f; // avg with color tint because why not
	
	float3 finalColor = float3(0, 0, 0);
	finalColor += EvaluateLight(l1, input.normal, input.color, input.worldPos, input.cameraPos, specInt).rgb;
	finalColor += EvaluateLight(l2, input.normal, input.color, input.worldPos, input.cameraPos, specInt).rgb;
	finalColor += EvaluateLight(l3, input.normal, input.color, input.worldPos, input.cameraPos, specInt).rgb;
	finalColor += EvaluateLight(l4, input.normal, input.color, input.worldPos, input.cameraPos, specInt).rgb;
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(finalColor,1);
	
}
