#include "Material.h"

Material::Material(SimplePixelShader *pixelShader, SimpleVertexShader *vertexShader, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap, ID3D11SamplerState* sampler, DirectX::XMFLOAT4 tint)
{
	this->pixelShader = pixelShader;
	this->vertexShader = vertexShader;
	this->texture = texture;
	this->sampler = sampler;
	this->tint = tint;

	if (normalMap == nullptr) 
	{
		hasNormals = false;
	}
	else 
	{
		hasNormals = true;
		this->normalMap = normalMap;
	}
}

DirectX::XMFLOAT4 Material::GetTint()
{
	return tint;
}

void Material::SetTint(DirectX::XMFLOAT4 tint)
{
	this->tint = tint;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView* Material::GetTexture()
{
	return texture.Get();
}

ID3D11ShaderResourceView* Material::GetNormalMap()
{
	return normalMap.Get();;
}

ID3D11SamplerState* Material::GetSampler()
{
	return sampler.Get();
}

bool Material::HasNormals()
{
	return hasNormals;
}
