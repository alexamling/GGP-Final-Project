#include "Bullet.h"

Bullet::Bullet(Mesh* mesh, SimplePixelShader* pixelShader, float spec, float rad, SimpleVertexShader* vertexShader, XMFLOAT4 tintInput, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity)
: Entity { mesh, pixelShader, spec, rad, vertexShader, tintInput, diffuseTexture, normalMap, sampOpt }
{
	this->velocity = velocity;
}

Bullet::~Bullet()
{
}

void Bullet::Update(float dt)
{
	entityTrans->MoveAbsolute(dt * XMLoadFloat3(&velocity));
}
