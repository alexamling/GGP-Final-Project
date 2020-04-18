#include "Asteroid.h"

Asteroid::Asteroid(Mesh* mesh, SimplePixelShader* pixelShader, float spec, float rad, SimpleVertexShader* vertexShader, XMFLOAT4 tintInput, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity, int size)
	: Entity{ mesh, pixelShader, spec, rad, vertexShader, tintInput, diffuseTexture, normalMap, sampOpt }
{
	asteroidSize = size;
	entityTrans->SetPosition(position.x, position.y, position.z);
	entityVelocity = velocity;
}

bool Asteroid::Split()
{
	asteroidSize -= 1;
	if (asteroidSize <= 0)
	{
		return false;
	}
	else
	{
		entityTrans->SetScale(1, 1, 1);
		return true;
	}
}

void Asteroid::Update(float deltaTime,XMVECTOR position,float playerRadius)
{
	entityTrans->MoveAbsolute(entityVelocity.x * deltaTime, entityVelocity.y * deltaTime, entityVelocity.z * deltaTime);
	checkCollision(position,playerRadius);
}

XMFLOAT3 Asteroid::GetVelocity()
{
	return entityVelocity;
}

void Asteroid::SetVelocity(XMFLOAT3 velocity)
{
	entityVelocity = velocity;
}
