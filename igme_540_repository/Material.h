#pragma once

#include <wrl/client.h>
#include "DXCore.h"
#include <DirectXMath.h>
#include "SimpleShader.h"

using namespace DirectX;

class Material
{
public:
	Material(XMVECTOR tint, float spec, SimplePixelShader* pixelShader, SimpleVertexShader* vertexShader, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions);

	SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();

	void SetColorTint(XMVECTOR tint);
	XMFLOAT4 GetColorTint();

	void SetSpec(float spec);
	float GetSpec();

	void SetDiffuseTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetDiffuseTexure();


private:
	XMFLOAT4 colorTint;
	float specular;

	// Shaders and shader-related constructs
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
};

