#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include "SimpleShader.h"
#include "WICTextureLoader.h"

class Material
{
private:
	// fields
	DirectX::XMFLOAT4 color;
	SimpleVertexShader* vertShader;
	SimplePixelShader* pixShader;
	ID3D11ShaderResourceView* diffuse;
	ID3D11ShaderResourceView* normal;
	ID3D11SamplerState* sampState;

	float specInt;

public:
	// constructor
	Material(DirectX::XMFLOAT4 color, SimpleVertexShader* vertShader, SimplePixelShader* pixShader, ID3D11ShaderResourceView* diffuse, ID3D11SamplerState* sampState, float specInt);
	Material(DirectX::XMFLOAT4 color, SimpleVertexShader* vertShader, SimplePixelShader* pixShader, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal, ID3D11SamplerState* sampState, float specInt);

private:
	// constructor helpers
	void Init(DirectX::XMFLOAT4 color, SimpleVertexShader* vertShader, SimplePixelShader* pixShader, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal, ID3D11SamplerState* sampState, float specInt);

public:
	// getters
	DirectX::XMFLOAT4 GetColor();
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetDiffuse();
	ID3D11ShaderResourceView* GetNormal();
	ID3D11SamplerState* GetSamplerState();
	float GetSpecInt();

	// setters
	void SetColor(DirectX::XMFLOAT4 color);
};

