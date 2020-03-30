#pragma once

#include "DXCore.h"
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>
#include "light.h"
#include "WICTextureLoader.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Mode, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	std::unique_ptr<Camera> camera;

	std::vector<GameEntity> entities;

	// Shaders and shader-related constructs
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader_Normals;
	SimpleVertexShader* vertexShader_Normals;
	//std::shared_ptr<SimplePixelShader> pixelShader;
	//std::shared_ptr<SimpleVertexShader> vertexShader;

	Mesh* mesh1;
	Mesh* mesh2;

	ID3D11ShaderResourceView* texture1;
	ID3D11ShaderResourceView* texture2;
	ID3D11ShaderResourceView* normalMap1;
	ID3D11ShaderResourceView* normalMap2;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	DirectionalLight dirLight1;
	DirectionalLight dirLight2;
	DirectionalLight dirLight3;

	PointLight pointLight1;
	PointLight pointLight2;

	float specExponent;
};

