#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
#include "Material.h"

class GameEntity
{
public:
	GameEntity(Mesh* mesh, Material* material);

	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

