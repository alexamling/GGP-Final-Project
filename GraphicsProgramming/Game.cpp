#include "Game.h"
#include "Vertex.h"
#include "BufferStructs.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	mesh1 = nullptr;
	mesh2 = nullptr;
	pixelShader = nullptr;
	vertexShader = nullptr;
	texture1 = nullptr;
	texture2 = nullptr;
	specExponent = 32;

	dirLight1.ambientColor = DirectX::XMFLOAT3(.0f, .0f, .0f);
	dirLight1.diffuseColor = DirectX::XMFLOAT3(0, .5f, 0);
	dirLight1.direction = DirectX::XMFLOAT3(1, 0, 0);

	dirLight2.ambientColor = DirectX::XMFLOAT3(.0f, .0f, .0f);
	dirLight2.diffuseColor = DirectX::XMFLOAT3(.5f, 0, 0);
	dirLight2.direction = DirectX::XMFLOAT3(0, 0, 1);

	dirLight3.ambientColor = DirectX::XMFLOAT3(.0f, .0f, .0f);
	dirLight3.diffuseColor = DirectX::XMFLOAT3(0, .5f, 0);
	dirLight3.direction = DirectX::XMFLOAT3(0, -1, 0);


	pointLight1.ambientColor = DirectX::XMFLOAT3(.05f, .05f, .05f);
	pointLight1.diffuseColor = DirectX::XMFLOAT3(.5f, 0, .5f);
	pointLight1.position = DirectX::XMFLOAT3(-1, -1, -1);

	pointLight2.ambientColor = DirectX::XMFLOAT3(.05f, .05f, .05f);
	pointLight2.diffuseColor = DirectX::XMFLOAT3(0, .5f, .5f);
	pointLight2.position = DirectX::XMFLOAT3(-1, -1, -1);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object
	delete pixelShader;
	delete vertexShader;
	delete pixelShader_Normals;
	delete vertexShader_Normals;
	delete mesh1;
	delete mesh2;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{

	Transform camTransform = Transform();
	camTransform.MoveAbsolute(0, 0, -5);
	camera = std::make_unique<Camera>(Camera(camTransform, (float)this->width / this->height, 90, .01f, 100.0f, 1.0f, 5.0f));

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();

	pixelShader->SetData("directionalLight1", &dirLight1, sizeof(DirectionalLight));
	pixelShader->SetData("directionalLight2", &dirLight2, sizeof(DirectionalLight));
	pixelShader->SetData("directionalLight3", &dirLight3, sizeof(DirectionalLight));
	pixelShader->SetData("pointLight1", &pointLight1, sizeof(PointLight));
	pixelShader->SetData("pointLight2", &pointLight2, sizeof(PointLight));
	pixelShader->SetData("specExponent", &specExponent, sizeof(float));
	pixelShader->CopyAllBufferData();

	pixelShader_Normals->SetData("directionalLight1", &dirLight1, sizeof(DirectionalLight));
	pixelShader_Normals->SetData("directionalLight2", &dirLight2, sizeof(DirectionalLight));
	pixelShader_Normals->SetData("directionalLight3", &dirLight3, sizeof(DirectionalLight));
	pixelShader_Normals->SetData("pointLight1", &pointLight1, sizeof(PointLight));
	pixelShader_Normals->SetData("pointLight2", &pointLight2, sizeof(PointLight));
	pixelShader_Normals->SetData("specExponent", &specExponent, sizeof(float));
	pixelShader_Normals->CopyAllBufferData();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = new SimplePixelShader(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	vertexShader_Normals = new SimpleVertexShader(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"VertexShader_Normals.cso").c_str());
	pixelShader_Normals = new SimplePixelShader(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"PixelShader_Normals.cso").c_str());
	/*
	vertexShader = std::make_shared<SimpleVertexShader>(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	*/
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../../Assets/Textures/TexturesCom_BrickMessy0005_1_seamless_S.jpg").c_str(), nullptr, &texture1);
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../../Assets/Textures/cushion.png").c_str(), nullptr, &texture2);
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../../Assets/Textures/cushion_normals.png").c_str(), nullptr, &normalMap2);

	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.MaxAnisotropy = 8;
	device->CreateSamplerState(&desc, samplerState.GetAddressOf());

	Material mat1 = Material(pixelShader, vertexShader, texture1, nullptr, samplerState.Get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Material mat2 = Material(pixelShader_Normals, vertexShader_Normals, texture2, normalMap2, samplerState.Get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	mesh1 = new Mesh(GetFullPathTo("../../../Assets/Models/sphere.obj").c_str(), device);
	mesh2 = new Mesh(GetFullPathTo("../../../Assets/Models/helix.obj").c_str(), device);

	// Create a color to default to -----------------------------------
	DirectX::XMFLOAT4 defaultColor = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);


	entities.push_back(GameEntity(mesh1, &mat1));
	//entities.push_back(GameEntity(mesh1, &mat1));
	entities.push_back(GameEntity(mesh2, &mat2));
	//entities.push_back(GameEntity(mesh2, &mat2));
	//entities.push_back(GameEntity(mesh2, &mat2));
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	if (camera != nullptr) camera->UpdateProjectionMatrix((float)this->width / this->height);
	DXCore::OnResize();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	entities[0].GetTransform()->Rotate(0, 0, .1f * deltaTime);
	entities[0].GetTransform()->MoveAbsolute(-.01f * deltaTime, 0, 0);
	entities[1].GetTransform()->MoveAbsolute(.01f * deltaTime, 0, 0);
	//entities[2].GetTransform()->Rotate(0, -.1f * deltaTime, 0);
	//entities[3].GetTransform()->MoveAbsolute(-.01f * deltaTime, 0, 0);

	camera->Update(deltaTime, this->hWnd);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	//  - for this demo, this step *could* simply be done once during Init(),
	//    but I'm doing it here because it's often done multiple times per frame
	//    in a larger application/game
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (int i = 0; i < entities.size(); i++)
	{
		SimpleVertexShader* vs = entities[i].GetMaterial()->GetVertexShader();
		vs->SetFloat4("colorTint", entities[i].GetMaterial()->GetTint());
		vs->SetMatrix4x4("world", entities[i].GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", camera.get()->GetViewMatrix());
		vs->SetMatrix4x4("proj", camera.get()->GetProjectionMatrix());
		vs->CopyAllBufferData();

		SimplePixelShader* ps = entities[i].GetMaterial()->GetPixelShader();
		ps->SetSamplerState("samplerOptions", entities[i].GetMaterial()->GetSampler());
		ps->SetShaderResourceView("baseTexture", entities[i].GetMaterial()->GetTexture());
		if (entities[i].GetMaterial()->HasNormals()) 
		{
			ps->SetShaderResourceView("normalMap", entities[i].GetMaterial()->GetNormalMap());
		}
		ps->SetData("camPos", &camera.get()->GetPos(), sizeof(DirectX::XMFLOAT3));
		ps->CopyAllBufferData();

		entities[i].GetMaterial()->GetVertexShader()->SetShader();
		entities[i].GetMaterial()->GetPixelShader()->SetShader();

		context->IASetVertexBuffers(0, 1, entities[i].GetMesh()->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(entities[i].GetMesh()->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			entities[i].GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted) TODO: CHANGE THIS WHEN ADDING NEW OBJECTS
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}