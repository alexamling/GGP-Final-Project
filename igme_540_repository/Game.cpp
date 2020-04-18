#include "Game.h"

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
	delete vertexShader;
	delete pixelShader;
	delete MeshOne;
	delete MeshTwo;
	delete MeshThree;
	for (int i = 0; i < asteroids.size(); i++)
	{
		delete asteroids[i];
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}
	delete MainCamera;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// cursor stuff
	ShowCursor(FALSE);
	RECT r;
	GetWindowRect(this->hWnd, &r);
	SetCursorPos(width / 2 + r.left, height / 2 + r.top);

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR ori = XMVectorSet(0, 0, 0, 0);

	MainCamera = new Camera(XMFLOAT3(0, 0, -2), XMFLOAT3(0, 0, 0), 70, this->width, this->height, .01f, 100.f, 5, 2, .5f, hWnd);

	dirLight.DiffuseColor = XMFLOAT3(0.8f, 0.8f, 0.8f);
	dirLight.Direction = XMFLOAT3(1,-1, 0);
	dirLight.Intensity = 10.0f;

	pntLight.Color = XMFLOAT3(0.7f, 0.7f, 0.7f);
	pntLight.Intensity = 1.0f;
	pntLight.Range = 10.0f;

	// Texture releated init
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),	// Passing in the context auto-generates mipmaps!!
		GetFullPathTo_Wide(L"../../Assets/Textures/brick.png").c_str(),
		nullptr,		// We don't need the texture ref ourselves
		diffuseTexture.GetAddressOf()); // We do need an SRV

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),	// Passing in the context auto-generates mipmaps!!
		GetFullPathTo_Wide(L"../../Assets/Textures/brick_normal.png").c_str(),
		nullptr,		// We don't need the texture ref ourselves
		normalMap.GetAddressOf()); // We do need an SRV


	// Describe the sampler state that I want
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, samplerOptions.GetAddressOf());

	// Sprite batch setup (and sprite font loading)
	spriteBatch = std::make_unique<SpriteBatch>(context.Get());
	spriteFont = std::make_unique<SpriteFont>(device.Get(), GetFullPathTo_Wide(L"../../Assets/Fonts/Arial.spritefont").c_str());

	// Prepare our sky resources ----------------------------
	skyMesh = MeshThree;

	skySRV = CreateCubemap(
		GetFullPathTo_Wide(L"../../Assets/Stars/right.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Stars/left.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Stars/up.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Stars/down.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Stars/forward.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Stars/back.png").c_str());

	// Make the sky rasterizer state
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rastDesc, skyRasterState.GetAddressOf());

	// Make the sky depth state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dsDesc, skyDepthState.GetAddressOf());
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
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str()); 
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	skyVS = new SimpleVertexShader(device.Get(),context.Get(),GetFullPathTo_Wide(L"SkyVS.cso").c_str());
	skyPS = new SimplePixelShader(device.Get(),context.Get(),GetFullPathTo_Wide(L"SkyPS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent normals and UV
	// - Not necessary, just makes things more readable
	XMFLOAT3 tempNormal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	XMFLOAT3 tempTangent = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT2 tempUV = XMFLOAT2(0.0f,0.0f);

	MeshOne = new Mesh(device, context, GetFullPathTo("../../Assets/Models/sphere.obj").c_str());

	MeshTwo = new Mesh(device,context,GetFullPathTo("../../Assets/Models/helix.obj").c_str());

	MeshThree = new Mesh(device, context, GetFullPathTo("../../Assets/Models/cube.obj").c_str());

	XMFLOAT4 red = XMFLOAT4(0.7f, 0.0f, 0.0f,0);
	XMFLOAT4 green = XMFLOAT4(0.0f, 0.7f, 0.0f,0);
	XMFLOAT4 blue = XMFLOAT4(0.1f, 0.1f, 0.7f,0);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f,0);
	XMFLOAT4 white = XMFLOAT4(0.7f, 0.7f, 0.7f,0);

	XMFLOAT3 pos;
	XMFLOAT3 vel;
	srand(time(0));
	for (int i = 0; i < 256; i++)
	{
		pos.x = (rand() % 50) - 25;
		pos.y = (rand() % 50) - 25;
		pos.z = (rand() % 50) - 25;
		vel.x = (rand() % 2) - 1;
		vel.y = (rand() % 2) - 1;
		vel.z = (rand() % 2) - 1;
		asteroids.push_back(new Asteroid(
			MeshOne, pixelShader, 10.0f, 0.75f, 
			vertexShader, white, 
			diffuseTexture, normalMap, 
			samplerOptions, pos, vel));
	}
}

bool Game::SplitAsteroid(int index)
{
	bool split = asteroids[index]->Split();
	if (split)
	{
		XMFLOAT3 vel;
		vel.x = (rand() % 2) - 1;
		vel.y = (rand() % 2) - 1;
		vel.z = (rand() % 2) - 1;
		// create new asteroid from old asteroid
		asteroids.push_back(new Asteroid(MeshOne, pixelShader, 10.0f, 0.75f, 
			vertexShader, XMFLOAT4(0.7f, 0.7f, 0.7f,0),
			diffuseTexture, normalMap, 
			samplerOptions, asteroids[index]->GetTransform()->GetPosition(), vel, asteroids[index]->asteroidSize));
		// change asteroid velocity
		vel.x *= -1;
		vel.y *= -1;
		vel.z *= -1;
		asteroids[index]->SetVelocity(vel);
	}
	else
	{
		asteroids.erase(asteroids.begin() + index);
	}
	return split;
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	if (MainCamera != nullptr) {
		MainCamera->UpdateProjectionMatrix(this->width, this->height, hWnd);
	}
}

// --------------------------------------------------------
// Loads six individual textures (the six faces of a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face.  Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not the SHADER RESOURCE VIEWS!
	// - Specifically NOT generating mipmaps, as we don't need them for the sky!
	ID3D11Texture2D* textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)&textures[0], 0);
	CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)&textures[1], 0);
	CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)&textures[2], 0);
	CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)&textures[3], 0);
	CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)&textures[4], 0);
	CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)&textures[5], 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array".  This is a special GPU resource format, 
	// NOT just a C++ arrray of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;  // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This should be treated as a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the actual texture resource
	ID3D11Texture2D* cubeMapTexture = 0;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,	// Which mip (zero, since there's only one)
			i,	// Which array element?
			1); // How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture, // Destination resource
			subresource,	// Dest subresource index (one of the array elements)
			0, 0, 0,		// XYZ location of copy
			textures[i],	// Source resource
			0,				// Source subresource index (we're assuming there's only one)
			0);				// Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;	// Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture, &srvDesc, cubeSRV.GetAddressOf());

	// Now that we're done, clean up the stuff we don't need anymore
	cubeMapTexture->Release();  // Done with this particular reference (the SRV has another)
	for (int i = 0; i < 6; i++)
		textures[i]->Release();

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}

// --------------------------------------------------------
// Perform all the steps necessary to render the sky
// --------------------------------------------------------
void Game::RenderSky()
{
	// Set up my render states
	context->RSSetState(skyRasterState.Get());
	context->OMSetDepthStencilState(skyDepthState.Get(), 0);

	// Set up the sky shaders
	skyVS->SetShader();
	skyPS->SetShader();

	skyVS->SetMatrix4x4("view", MainCamera->GetViewMatrix());
	skyVS->SetMatrix4x4("proj", MainCamera->GetProjectionMatrix());
	skyVS->CopyAllBufferData();

	skyPS->SetShaderResourceView("sky", skySRV.Get());
	skyPS->SetSamplerState("samplerOptions", samplerOptions.Get());

	// Actually draw the geometry
	skyMesh->SetBuffers();
	skyMesh->DrawMesh();

	// Reset any states back to the default
	context->RSSetState(0); // Sets the default state
	context->OMSetDepthStencilState(0, 0);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	for (int i = 0; i < asteroids.size(); i++) 
	{
		asteroids[i]->Update(deltaTime,XMLoadFloat3(&MainCamera->GetTransform()->GetPosition()),0.75f);
		if (asteroids[i]->colliding)
		{
			SplitAsteroid(i);
		}
	}

	MainCamera->Update(deltaTime,this->hWnd);
	pntLight.Position = MainCamera->GetTransform()->GetPosition();

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		XMFLOAT3 vel = XMFLOAT3(0, 0, 10.f);
		XMStoreFloat3(&vel, XMVector3Rotate(XMLoadFloat3(&vel), XMLoadFloat4(&(MainCamera->GetTransform()->GetPitchYawRoll()))));
		Bullet* b = new Bullet(MeshOne, pixelShader, 10.0f, 0.1f, vertexShader, XMFLOAT4(1, 1, 1, 1), diffuseTexture, normalMap, samplerOptions, MainCamera->GetTransform()->GetPosition(), vel, XMFLOAT3(.1f,.1f,.1f));
		b->GetTransform()->SetRotation(MainCamera->GetTransform()->GetPitchYawRoll());
		bullets.push_back(b);
	}

	for (int i = 0; i < bullets.size(); i++) {
		bullets[i]->Update(deltaTime);
	}
	
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.5f, 0.9f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vertexShader->SetShader();
	pixelShader->SetShader();

	pixelShader->SetData("dirLight",	// The name of the (eventual) variable in the shader
		&dirLight,									// The address of the data to set 
		sizeof(DirectionalLight));				// The size of the data to set

	pixelShader->SetData("pntLight",	// The name of the (eventual) variable in the shader
		&pntLight,									// The address of the data to set 
		sizeof(PointLight));				// The size of the data to set

	pixelShader->SetFloat3("AmbientColor",XMFLOAT3(0.1f, 0.1f, 0.1f));
	pixelShader->SetFloat3("cameraPosition", MainCamera->GetTransform()->GetPosition());

	// Set texture resources for the next draw
	pixelShader->SetShaderResourceView("diffuseTexture", diffuseTexture.Get());
	pixelShader->SetShaderResourceView("normalMap", normalMap.Get());
	pixelShader->SetSamplerState("samplerOptions", samplerOptions.Get());

	for (int i = 0; i < asteroids.size(); i++)
	{
		pixelShader->SetFloat("Specularity", asteroids[i]->GetMaterial()->GetSpec());
		pixelShader->CopyAllBufferData();
		asteroids[i]->Draw(MainCamera);
	}

	for (int i = 0; i < bullets.size(); i++)
	{
		pixelShader->SetFloat("Specularity", bullets[i]->GetMaterial()->GetSpec());
		pixelShader->CopyAllBufferData();
		bullets[i]->Draw(MainCamera);
	}

	RenderSky();

	// === SpriteBatch =====================================
	// See these links for more info!
	// SpriteBatch: https://github.com/microsoft/DirectXTK/wiki/SpriteBatch
	// SpriteFont: https://github.com/microsoft/DirectXTK/wiki/SpriteFont
	{

		// Grab the SRV of the font from the SpriteFont
		// Note: It's not great to do this every frame, but 
		// this is just a demo to show what it looks like!
		ID3D11ShaderResourceView* fontSheet;
		spriteFont->GetSpriteSheet(&fontSheet);

		// Begin the batch
		spriteBatch->Begin();

		// Draw the text score
		spriteFont->DrawString(
			spriteBatch.get(),
			L"Score: 00",
			XMFLOAT2((float)(this->width)/2, 20));

		// Done with the batch
		spriteBatch->End();

		// Release the extra reference to the font sheet we made above
		// when we called GetSpriteSheet()
		fontSheet->Release();

		// Reset any states that may be changed by sprite batch!
		context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		context->RSSetState(0);
		context->OMSetDepthStencilState(0, 0);
	}
	// ======================================================

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}