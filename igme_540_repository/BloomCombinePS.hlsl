
cbuffer externalData : register(b0)
{
	float intensityLevel0;
	float intensityLevel1;
}


// Defines the input to this pixel shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and such
Texture2D originalPixels	: register(t0);
Texture2D bloomedPixels0	: register(t1);
Texture2D bloomedPixels1	: register(t2);
SamplerState samplerOptions	: register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 totalColor = originalPixels.Sample(samplerOptions, input.uv);
	totalColor += bloomedPixels0.Sample(samplerOptions, input.uv) * intensityLevel0;
	totalColor += bloomedPixels1.Sample(samplerOptions, input.uv) * intensityLevel1;

	//originalColor *= (1 - saturate(bloomedColor));

	return totalColor;
}