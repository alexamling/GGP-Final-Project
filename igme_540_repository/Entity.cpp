#include "Entity.h"

Entity::Entity(Mesh* mesh, SimplePixelShader* pixelShader, float spec, float rad,
	SimpleVertexShader* vertexShader, XMFLOAT3 tintInput,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt)
{
	entityTrans = new Transform();
	
	entityMesh = mesh;
	
	mat = new Material(XMLoadFloat3(&tintInput), spec, pixelShader, vertexShader, diffuseTexture,normalMap,sampOpt);

	radius = rad;

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
	SimpleVertexShader* vs = Entity::mat->GetVertexShader(); //   Simplifies next few lines 
	vs->SetFloat3("color", mat->GetColorTint());
	vs->SetMatrix4x4("world", entityTrans->GetWorldMatrix());
	vs->SetMatrix4x4("view", mainCamera->GetViewMatrix());
	vs->SetMatrix4x4("proj", mainCamera->GetProjectionMatrix());

	vs->CopyAllBufferData();

	entityMesh->SetBuffers();
	entityMesh->DrawMesh();
}

bool Entity::checkCollision(XMVECTOR position, float playerRadius)
{
	float bounds = (this->radius + playerRadius);
	XMVECTOR vectorSub = XMVectorSubtract(XMLoadFloat3(&entityTrans->GetPosition()), position);
	XMVECTOR length = XMVector3Length(vectorSub);

	float distance = 0.0f;
	XMStoreFloat(&distance, length);
	
	if (distance > bounds)
	{
		XMFLOAT3 newTint = XMFLOAT3(0, 1, 0);
		mat->SetColorTint(XMLoadFloat3(&newTint));
		return false;
	}
	else {
		XMFLOAT3 newTint = XMFLOAT3(1,0,0);
		mat->SetColorTint(XMLoadFloat3(&newTint));
		return true;
	}
}
