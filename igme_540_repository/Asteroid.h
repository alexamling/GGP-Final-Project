#pragma once
#include "Entity.h"

class Asteroid : public Entity
{
	public:
		Asteroid(Mesh* mesh, SimplePixelShader* pixelShader, float spec,
			SimpleVertexShader* vertexShader, XMFLOAT4 tintInput,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap,
			Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt, XMFLOAT3 position, XMFLOAT3 velocity);
		void Split(Asteroid* children);
	private:
		int size;
};

