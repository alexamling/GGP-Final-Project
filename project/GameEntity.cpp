#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh, Material* material)
{
	transform = Transform();
	this->mesh = mesh;
	this->material = material;
}

Transform* GameEntity::GetTransform()
{
	return &transform;
}

Mesh* GameEntity::GetMesh()
{
	return mesh;
}

Material* GameEntity::GetMaterial()
{
	return material;
}
