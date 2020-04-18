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
	for (int i = 0; i < 5; i++)
	{
		delete entityArr[i];
	}
	delete MainCamera;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
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

	MainCamera = new Camera(pos,ori,(float)(this->width/this->height),90,0.01f,100.0f,3.0f,2.0f);

	dirLight.DiffuseColor = XMFLOAT3(0.8f, 0.8f, 0.8f);
	dirLight.Direction = XMFLOAT3(0, 3, 0);
	dirLight.Intensity = 1.0f;

	pntLight.Color = XMFLOAT3(0.7f, 0.7f, 0.7f);
	pntLight.Position = XMFLOAT3(0, 10, 5);
	pntLight.Intensity = 1.0f;
	pntLight.Range = 2.0f;

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

	MeshOne = new Mesh(device, context, GetFullPathTo("../../Assets/Models/helix.obj").c_str());

	//Smaller Triangle
	MeshTwo = new Mesh(device,context,GetFullPathTo("../../Assets/Models/sphere.obj").c_str());

	Vertex vertices[] =
	{
		{ XMFLOAT3(-10.0f, -2.0f, -10.0f), tempNormal, tempTangent, tempUV },
		{ XMFLOAT3(-10.0f, -2.0f, +10.0f), tempNormal, tempTangent, tempUV },
		{ XMFLOAT3(+10.0f, -2.0f, +10.0f), tempNormal, tempTangent, tempUV },
		{ XMFLOAT3(+10.0f, -2.0f, -10.0f), tempNormal, tempTangent, tempUV }
	};

	UINT indices2[] = {0,1,3,1,2,3};
	//This one is a square
	MeshThree = new Mesh(vertices, 4, indices2, 6, device, context);

	XMFLOAT4 red = XMFLOAT4(0.7f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 0.7f, 0.0f, 0.0f);
	XMFLOAT4 blue = XMFLOAT4(0.1f, 0.1f, 0.7f, 0.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 white = XMFLOAT4(0.7f, 0.7f, 0.7f, 0.0f);

	entityArr[0] = new Entity(MeshOne, pixelShader, 10.0f, vertexShader, white, diffuseTexture, normalMap, samplerOptions);
	entityArr[0]->GetTransform()->SetScale(0.25f, 0.25f, 0.25f);
	entityArr[1] = new Entity(MeshOne, pixelShader, 64.0f, vertexShader, red, diffuseTexture, normalMap, samplerOptions);
	entityArr[1]->GetTransform()->SetScale(0.25f, 0.25f, 0.25f);

	entityArr[2] = new Entity(MeshTwo, pixelShader, 45.0f, vertexShader, green, diffuseTexture, normalMap, samplerOptions);
	entityArr[3] = new Entity(MeshTwo, pixelShader, 64.0f, vertexShader, blue, diffuseTexture, normalMap, samplerOptions);
	entityArr[4] = new Entity(MeshThree, pixelShader, 64.0f, vertexShader, black, diffuseTexture, normalMap, samplerOptions);

}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	MainCamera->UpdateProjectionMatrix((float)(this->width/this->height));
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{

	entityArr[0]->GetTransform()->SetPosition(cos(totalTime)*2,0,0);
	entityArr[0]->GetTransform()->RotateAbsolute(deltaTime, 0, 0);
	//entityArr[1]->GetTransform()->Rotate(0, deltaTime, 0);
	entityArr[2]->GetTransform()->SetPosition(cos(totalTime),0,sin(totalTime));
	entityArr[3]->GetTransform()->SetPosition(-cos(totalTime), sin(totalTime), 0);
	MainCamera->Update(deltaTime,this->hWnd);

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
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

	for (int i = 0; i < 5; i++)
	{
		pixelShader->SetFloat("Specularity", entityArr[i]->GetMaterial()->GetSpec());
		pixelShader->CopyAllBufferData();
		entityArr[i]->Draw(MainCamera);
	}

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
			XMFLOAT2(10, 120));

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