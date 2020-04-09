#pragma once

#include <wrl/client.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"

class Entity
{
public:
	Entity(Mesh* mesh, SimplePixelShader* pixelShader, float spec,
		SimpleVertexShader* vertexShader,XMFLOAT4 tintInput,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt);
	~Entity();

	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();

	void Draw(Camera* mainCamera);

private:
	Mesh* entityMesh;
	Transform* entityTrans;
	Material* mat;
};

