#include "Asteroid.h"

Asteroid::Asteroid(Mesh* mesh, SimplePixelShader* pixelShader, float spec, SimpleVertexShader* vertexShader, XMFLOAT4 tintInput, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity)
	: Entity{ mesh, pixelShader, spec, vertexShader, tintInput, diffuseTexture, normalMap, sampOpt }
{
	Asteroid::size = 3;

}

void Asteroid::Split(Asteroid* children)
{
}
