#include "Material.h"

Material::Material(DirectX::XMFLOAT4 color, SimpleVertexShader* vertShader, SimplePixelShader* pixShader, ID3D11ShaderResourceView* diffuse, ID3D11SamplerState* sampState, float specInt)
{
	Init(color, vertShader, pixShader, diffuse, nullptr, sampState, specInt);
}

Material::Material(DirectX::XMFLOAT4 color, SimpleVertexShader* vertShader, SimplePixelShader* pixShader, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal, ID3D11SamplerState* sampState, float specInt)
{
	Init(color, vertShader, pixShader, diffuse, normal, sampState, specInt);
}

void Material::Init(DirectX::XMFLOAT4 color, SimpleVertexShader* vertShader, SimplePixelShader* pixShader, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal, ID3D11SamplerState* sampState, float specInt)
{
	this->color = color;
	this->vertShader = vertShader;
	this->pixShader = pixShader;

	this->diffuse = diffuse;
	this->normal = normal;
	this->sampState = sampState;

	this->specInt = specInt;
}

DirectX::XMFLOAT4 Material::GetColor()
{
	return color;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixShader;
}

ID3D11ShaderResourceView* Material::GetDiffuse()
{
	return diffuse;
}

ID3D11ShaderResourceView* Material::GetNormal()
{
	return normal;
}

ID3D11SamplerState* Material::GetSamplerState()
{
	return sampState;
}

float Material::GetSpecInt()
{
	return specInt;
}

void Material::SetColor(DirectX::XMFLOAT4 color)
{
	this->color = color;
}
