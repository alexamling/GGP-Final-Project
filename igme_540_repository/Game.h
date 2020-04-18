#pragma once

#include "DXCore.h"
#include <DirectXMath.h>

#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects

#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Asteroid.h"
#include "Bullet.h"
#include <time.h>

#include "Vertex.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SimpleShader.h"

#include "WICTextureLoader.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

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
	void SpawnAsteroids();

	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Mode, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
	
	Mesh* MeshOne; 
	Mesh* MeshTwo;
	Mesh* MeshThree;
	
	std::vector<Asteroid*> asteroids;
	std::vector<Bullet*> bullets;

	Camera* MainCamera;

	DirectionalLight dirLight;
	PointLight pntLight;

	// Texture related resources
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;

	// Sprite batch resources
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//variable for game score
	float score;
};