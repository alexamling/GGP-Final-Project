#include "Asteroid.h"

Asteroid::Asteroid(Mesh* mesh, SimplePixelShader* pixelShader, float spec, float rad, SimpleVertexShader* vertexShader, XMFLOAT4 tintInput, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity, int size)
	: Entity{ mesh, pixelShader, spec, rad, vertexShader, tintInput, diffuseTexture, normalMap, sampOpt }
{
	asteroidSize = size;
	entityTrans->SetPosition(position.x, position.y, position.z);
	float newScale = .3f * asteroidSize;
	entityTrans->SetScale(newScale, newScale, newScale);
	radius = .75f * newScale;
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
		float newScale = .3f * asteroidSize;
		entityTrans->SetScale(newScale, newScale, newScale);
		radius = .75f * newScale;
		return true;
	}
}

void Asteroid::Update(float deltaTime,XMVECTOR position,float playerRadius, std::vector<Bullet*>* bullets)
{
	entityTrans->MoveAbsolute(entityVelocity.x * deltaTime, entityVelocity.y * deltaTime, entityVelocity.z * deltaTime);
	CheckCollision(position,playerRadius,bullets);
}

void Asteroid::CheckCollision(XMVECTOR position, float playerRadius, std::vector<Bullet*>* bullets)
{
	for (int i = 0; i < bullets->size(); i++)
	{
		Bullet* bullet = bullets->at(i);
		float bounds = (this->radius + bullet->radius);
		XMVECTOR vectorSub = XMVectorSubtract(XMLoadFloat3(&entityTrans->GetPosition()), XMLoadFloat3(&bullet->GetTransform()->GetPosition()));
		XMVECTOR length = XMVector3Length(vectorSub);

		float distance = 0.0f;
		XMStoreFloat(&distance, length);

		if (distance > bounds)
		{
			//XMFLOAT4 newTint = XMFLOAT4(0, 1, 0, 0);
			//mat->SetColorTint(XMLoadFloat4(&newTint));
			colliding = false;
		}
		else {
			//XMFLOAT4 newTint = XMFLOAT4(1, 0, 0, 0);
			//mat->SetColorTint(XMLoadFloat4(&newTint));
			colliding = true;
			delete bullets->at(i);
			bullets->erase(bullets->begin() + i);
			break;
		}
	}
}

XMFLOAT3 Asteroid::GetVelocity()
{
	return entityVelocity;
}

void Asteroid::SetVelocity(XMFLOAT3 velocity)
{
	entityVelocity = velocity;
}
