#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__
// ALL of your code pieces (structs, functions, etc.) go here!
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float4 color		: COLOR;        // RGBA color
	float3 normal		: NORMAL;		// XYZ normal
	float3 worldPos		: POSITION;
	float3 tangent		: TANGENT;		// 
	float2 uv			: TEXCOORD;		// XY texture coordinate
};

struct DirectionalLight {
	float3 DiffuseColor;
	float3 Direction;
	float Intensity;
};

struct PointLight {
	float3 Position;
	float3 Color;
	float Intensity;
	float Range;
};

float3 Diffuse(float3 normal, float3 direction) {

	return saturate(dot(normal, -normalize(direction)));
}

float3 Specular(float3 normal, float3 Direction, float3 toCamera, float Specularity)
{
	// Calculate light reflection vector
	float3 refl = reflect(normalize(Direction), normal);

	return pow(saturate(dot(refl, toCamera)), Specularity);
}

float computeAttentuation(float3 lightPos, float3 pixelPos, float range) {
	float dist = distance(lightPos, pixelPos);
	float att = 1.0f - (dist * dist / (range * range));
	return att;
}

float3 computeColor(float3 normal, float3 dir, float3 worldPos, float3 lightColor, 
	float3 cameraPosition, float3 surfaceColor, float intensity, float specularity) {

	// Calculate the vector from the pixel's world position to the camera
	float3 toCamera = normalize(cameraPosition - worldPos);

	// DL 1
	float diffuse = Diffuse(normal, dir);
	float3 spec = Specular(normal, dir, toCamera, specularity);
	spec *= any(diffuse);

	float3 FinalColor = diffuse * intensity * lightColor * surfaceColor
		+ spec * intensity * lightColor;

	return FinalColor;
}

#endif