#pragma once
#include "Entity.h"
#include <DirectXMath.h>
#include "DXCore.h"

class Bullet : public Entity
{
private:
	DirectX::XMFLOAT3 velocity;

public:
	Bullet(Mesh* mesh, SimplePixelShader* pixelShader, float spec, float rad,
		SimpleVertexShader* vertexShader, XMFLOAT4 tintInput,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 scale);

	~Bullet();

	// methods
	void Update(float dt);
	float radius = .75f;
};

