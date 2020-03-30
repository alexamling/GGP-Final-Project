#pragma once
#include <DirectXMath.h>
#include "DXCore.h"
#include <wrl/client.h>
#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	Material(SimplePixelShader *pixelShader, SimpleVertexShader *vertexShader,ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap, ID3D11SamplerState* sampler, DirectX::XMFLOAT4 tint);

	DirectX::XMFLOAT4 GetTint();
	void SetTint(DirectX::XMFLOAT4);
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetNormalMap();
	ID3D11SamplerState* GetSampler();
	bool HasNormals();

private:
	SimpleVertexShader *vertexShader;
	SimplePixelShader *pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	//std::shared_ptr<SimpleVertexShader> vertexShader;
	//std::shared_ptr<SimplePixelShader> pixelShader;
	DirectX::XMFLOAT4 tint;
	bool hasNormals;
};

