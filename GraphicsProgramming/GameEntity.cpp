#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh, Material* material)
{
	this->mesh = std::make_shared<Mesh>(*mesh);
	transform = Transform();
	this->material = std::make_shared<Material>(*material);
}

Mesh* GameEntity::GetMesh()
{
	return mesh.get();
}

Transform* GameEntity::GetTransform()
{
	return &transform;
}

Material* GameEntity::GetMaterial()
{
	return material.get();
}
