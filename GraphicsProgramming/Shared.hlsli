#ifndef __GGP_SHADER_INCLUDES__
#define __GGPS_SHADER_INCLUDES__

struct VertexShaderInput
{
	float3 position		: POSITION;     // XYZ position
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float2 uv           : TEXCOORD;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 worldPos     : POSITION;
	float4 color		: COLOR;        // RGBA color
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
};

struct VertexToPixel_Normals
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 worldPos     : POSITION;
	float4 color		: COLOR;        // RGBA color
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float2 uv           : TEXCOORD;
};

struct DirectionalLight
{
	float3 ambientColor;
	float3 diffuseColor;
	float3 direction;
};

struct PointLight
{
	float3 ambientColor;
	float3 diffuseColor;
	float3 position;
};

float3 CalculateSpecular(float3 worldPos, float3 normal, float3 lightDir, float3 camPos, float specExponent)
{
	float3 V = normalize(camPos - worldPos);
	float3 R = reflect(lightDir, normal);
	return pow(saturate(dot(R, V)), max(specExponent, .001));
}

float3 CalculateLighting_Dir(float3 surfaceColor, float3 worldPos, float3 normal, DirectionalLight dirLight, float3 camPos, float specExponent)
{
	float3 toLight = normalize(-dirLight.direction);
	float diffuseAmount = dot(normal, toLight);
	float3 spec = CalculateSpecular(worldPos, normal, -toLight, camPos, specExponent);
	spec *= any(diffuseAmount);
	return float3(
		surfaceColor *
		(
			diffuseAmount * dirLight.diffuseColor   // Diffuse
			+ dirLight.ambientColor                 // Ambient
			+ spec // Specular
			)
		);
}

float3 CalculateLighting_Point(float3 surfaceColor, float3 worldPos, float3 normal, PointLight pointLight, float3 camPos, float specExponent)
{
	float3 toLight = normalize(pointLight.position - worldPos);
	float diffuseAmount = dot(normal, toLight);
	return float3(
		surfaceColor *
		(
			diffuseAmount * pointLight.diffuseColor   // Diffuse
			+ pointLight.ambientColor                 // Ambient
			+ CalculateSpecular(worldPos, normal, -toLight, camPos, specExponent)      // Specular
			)
		);
}

#endif