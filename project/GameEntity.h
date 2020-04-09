#pragma once

#include "DXCore.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

class GameEntity
{
	// fields
private:
	Transform transform;
	Mesh* mesh;
	Material* material;

	// Constructor
public:
	GameEntity(Mesh* mesh, Material* material);

	// Getters
	Transform* GetTransform();
	Mesh* GetMesh();
	Material* GetMaterial();
};

