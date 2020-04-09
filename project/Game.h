#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include "Transform.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h" 

#include "Lights.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

private:
	Mesh** meshes = nullptr;
	int meshesLength = 0;
	
	GameEntity** entities = nullptr;
	int entitiesLength = 0;

	Camera* camera = nullptr;

	Material** materials = nullptr;
	int materialsLength = 0;

	// since there can only be 3, no reason to loop/store in array
	Light dl1;
	Light dl2;
	Light dl3;

	// point light
	Light pl1;

	// textures
	ID3D11ShaderResourceView** textures = nullptr;
	int texturesLength = 0;
	int* texturesWithNorms = nullptr;

	ID3D11ShaderResourceView** normals = nullptr;
	int normalsLength = 0;

	ID3D11SamplerState* sampState = nullptr;

public:
	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void LoadTextures();
	void CreateMeshes();
	void CreateEntities();

	// other helper methods
	int IndexOf(int* arr, int arrLength, int val);
	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Mode, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	SimplePixelShader* pixelShader = nullptr;
	SimplePixelShader* normPixShader = nullptr;
	SimpleVertexShader* vertexShader = nullptr;
	SimpleVertexShader* normVertShader = nullptr;

};

