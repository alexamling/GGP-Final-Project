#include "Entity.h"

Entity::Entity(Mesh* mesh, SimplePixelShader* pixelShader, float spec,
	SimpleVertexShader* vertexShader, XMFLOAT4 tintInput,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt)
{
	entityTrans = new Transform();
	
	entityMesh = mesh;
	
	mat = new Material(XMLoadFloat4(&tintInput), spec, pixelShader, vertexShader, diffuseTexture,normalMap,sampOpt);

}

Entity::~Entity()
{
	delete entityTrans;
	delete mat;
}

Mesh* Entity::GetMesh()
{
	return entityMesh;
}

Transform* Entity::GetTransform()
{
	return entityTrans;
}

Material* Entity::GetMaterial()
{
	return mat;
}

void Entity::Draw( Camera* mainCamera)
{
	SimpleVertexShader* vs = mat->GetVertexShader(); //   Simplifies next few lines 
	vs->SetFloat4("color", mat->GetColorTint());
	vs->SetMatrix4x4("world", entityTrans->GetWorldMatrix());
	vs->SetMatrix4x4("view", mainCamera->GetViewMatrix());
	vs->SetMatrix4x4("proj", mainCamera->GetProjMatrix());

	vs->CopyAllBufferData();

	entityMesh->SetBuffers();
	entityMesh->DrawMesh();
}
