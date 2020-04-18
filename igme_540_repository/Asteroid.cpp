#include "Asteroid.h"

Asteroid::Asteroid(Mesh* mesh, SimplePixelShader* pixelShader, float spec, SimpleVertexShader* vertexShader, XMFLOAT4 tintInput, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity, int size)
	: Entity{ mesh, pixelShader, spec, vertexShader, tintInput, diffuseTexture, normalMap, sampOpt }
{
	asteroidSize = size;
	entityTrans->SetPosition(position.x, position.y, position.z);
	entityVelocity = velocity;
}

void Asteroid::Split(Asteroid newAsteroid)
{
}

void Asteroid::Update(float deltaTime)
{
	entityTrans->MoveAbsolute(entityVelocity.x * deltaTime, entityVelocity.y * deltaTime, entityVelocity.z * deltaTime);
}
