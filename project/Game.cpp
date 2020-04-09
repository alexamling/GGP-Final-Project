#include "Game.h"
#include "Vertex.h"
#include "RegularGenerator.h"
#include "WICTextureLoader.h"

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

	// make the camera
	camera = new Camera(XMFLOAT3(0, 0, -2), XMFLOAT3(0, 0, 0), 70, (float)this->width / this->height, .01f, 500.f, 1, 1, 10);

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

	// A2
	for (int i = 0; i < meshesLength; i++)
	{
		delete meshes[i];
	}
	delete meshes;
	
	// A3 / A6
	//vsConstantBuffer->Release();

	// A4
	for (int i = 0; i < entitiesLength; i++) {
		delete entities[i];
	}
	delete entities;

	// A5
	delete camera;

	for (int i = 0;  i < materialsLength; i++) {
		delete materials[i];
	}
	delete materials;

	// A6
	delete vertexShader;
	delete pixelShader;

	// A7
	for (int i = 0; i < texturesLength; i++) {
		textures[i]->Release();
	}
	delete textures;

	sampState->Release();

	// A8
	delete normVertShader;
	delete normPixShader;

	for (int i = 0; i < normalsLength; i++) {
		normals[i]->Release();
	}
	delete normals;

	delete[] texturesWithNorms;
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
	LoadTextures();
	CreateMeshes();
	CreateEntities();

	dl1 = Light();
	dl1.type = 0;
	dl1.ambientColor = XMFLOAT3(.1f, 0.f, 0.f);
	dl1.diffuseColor = XMFLOAT3(1, 0, 0.f);
	dl1.direction = XMFLOAT3(1, 0, 0);

	dl2 = Light();
	dl2.type = 0;
	dl2.ambientColor = XMFLOAT3(0.f, .1f, 0.f);
	dl2.diffuseColor = XMFLOAT3(0, 1, 0);
	dl2.direction = XMFLOAT3(0, 1, 0);

	dl3 = Light();
	dl3.type = 0;
	dl3.ambientColor = XMFLOAT3(0.f, 0.f, 0.1f);
	dl3.diffuseColor = XMFLOAT3(0, 0, 1);
	dl3.direction = XMFLOAT3(0, 0, 1);

	pl1 = Light();
	pl1.type = 1;
	pl1.ambientColor = XMFLOAT3(.1f, .1f, .1f);
	pl1.diffuseColor = XMFLOAT3(1.f, 1.f, 1.f);
	pl1.radius = 3.f;
	pl1.direction = XMFLOAT3(0.f, 2.f, 0.f);

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
	// Load with SimpleShaders
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), 
		GetFullPathTo_Wide(L"VertexShader.cso").c_str());

	normVertShader = new SimpleVertexShader(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"VertexShaderN.cso").c_str());

	pixelShader = new SimplePixelShader(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	
	normPixShader = new SimplePixelShader(device.Get(), context.Get(),
			GetFullPathTo_Wide(L"PixelShaderN.cso").c_str());
}

void Game::LoadTextures()
{
	// create temp paths to textures
	texturesLength = 4;
	std::wstring* paths = new std::wstring[texturesLength]{
		GetFullPathTo_Wide(L"../../Assets/Textures/brick.png"),
		GetFullPathTo_Wide(L"../../Assets/Textures/grass.png"),
		GetFullPathTo_Wide(L"../../Assets/Textures/cushion.png"),
		GetFullPathTo_Wide(L"../../Assets/Textures/rock.png")
	};

	textures = new ID3D11ShaderResourceView * [texturesLength];

	// load textures
	for (int i = 0; i < texturesLength; i++) {
		CreateWICTextureFromFile(
			device.Get(),
			context.Get(),
			paths[i].c_str(),
			nullptr,
			&textures[i]);
	}

	// link to normals (to be referenced when creating materials)
	normalsLength = 2;
	texturesWithNorms = new	int[normalsLength] { 2, 3 };

	delete[] paths;
	paths = new std::wstring[normalsLength]{
		GetFullPathTo_Wide(L"../../Assets/Textures/cushion_normals.png"),
		GetFullPathTo_Wide(L"../../Assets/Textures/rock_normals.png")
	};

	normals = new ID3D11ShaderResourceView * [normalsLength];

	// load normals
	for (int i = 0; i < normalsLength; i++) {
		CreateWICTextureFromFile(
			device.Get(),
			context.Get(),
			paths[i].c_str(),
			nullptr,
			&normals[i]);
	}

	// create sampler state
	D3D11_SAMPLER_DESC desc = D3D11_SAMPLER_DESC();
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(
		&desc,
		&sampState);

	// clean up temp paths
	delete[] paths;

	// create materials
	materialsLength = texturesLength;
	materials = new Material * [materialsLength];
	for (int i = 0; i < materialsLength; i++) {
		XMFLOAT4 color = XMFLOAT4((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.f);
		int norm = IndexOf(texturesWithNorms, normalsLength, i);
		if (norm != -1) {
			materials[i] = new Material(color, normVertShader, normPixShader, textures[i], normals[norm], sampState, 1.5f);
		}
		else {
			materials[i] = new Material(color, vertexShader, pixelShader, textures[i], sampState, 1.5f);
		}
	}
}

void Game::CreateMeshes()
{
	/*Vertex* verts = nullptr;
	unsigned int vertsLength = 0;
	unsigned int* indices = nullptr;
	unsigned int indicesLength = 0;

	meshesLength = 3;
	meshes = new Mesh * [meshesLength]; //thanks i hate it

	for (int i = 0; i < meshesLength; i++) {
		RegularGenerator::Generate(3 + i, XMFLOAT3(cosf(i * XM_2PI / meshesLength) * 0.5f, sinf(i * XM_2PI / meshesLength) * 0.5f, 0.0f),
			0.25f, verts, vertsLength, indices, indicesLength);

		if (vertsLength > 2 && indicesLength > 2)
		{
			meshes[i] = new Mesh(verts, vertsLength, indices, indicesLength, device);

			delete[] verts;
			verts = nullptr;
			vertsLength = 0;
			delete[] indices;
			indices = nullptr;
			indicesLength = 0;
		}
	}*/
	meshesLength = 6;
	std::string* paths = new std::string[meshesLength] {
		GetFullPathTo("../../Assets/Models/cone.obj"),
		GetFullPathTo("../../Assets/Models/cube.obj"),
		GetFullPathTo("../../Assets/Models/cylinder.obj"),
		GetFullPathTo("../../Assets/Models/helix.obj"),
		GetFullPathTo("../../Assets/Models/sphere.obj"),
		GetFullPathTo("../../Assets/Models/torus.obj")
	};

	meshes = new Mesh * [meshesLength];
	for  (int i = 0; i < meshesLength; i++)
	{
		meshes[i] = new Mesh(paths[i].c_str(), device);
	}

	// clean up temp paths
	delete[] paths;
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateEntities()
{
	// fun with entities
	entitiesLength = 6;
	entities = new GameEntity * [entitiesLength];
	for (int i = 0; i < entitiesLength; i++) {
		entities[i] = new GameEntity(meshes[i % meshesLength], materials[i % materialsLength]);
		entities[i]->GetTransform()->SetPosition((i - (float)entitiesLength / 2.f + .5f) * 2.f, 0.f, 0.f);
	}

	/* This might be important later
	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	int indices[] = { 0, 1, 2 };


	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 3;       // 3 = number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());



	// Create the INDEX BUFFER description ------------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * 3;         // 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	*/
}

int Game::IndexOf(int* arr, int arrLength, int val)
{
	for (int i = 0; i < arrLength; i++) {
		if (val == arr[i]) {
			return i;
		}
	}

	return -1;
}



// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	if (camera != nullptr) {
		camera->UpdateProjectionMatrix((float)this->width / this->height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// updating (You spin me right round...)
	for (int i = 0; i < entitiesLength; i++) {
		entities[i]->GetTransform()->RotateAbsolute(0, deltaTime * XM_PIDIV4 * (i + 1), 0);
	}

	// update the camera
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


	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	


	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	
	// A6 context->IASetInputLayout(inputLayout.Get());

	/*
	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	//  - for this demo, this step *could* simply be done once during Init(),
	//    but I'm doing it here because it's often done multiple times per frame
	//    in a larger application/game
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		3,     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
	*/

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer;

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

	// draw entities
	for (int i = 0; i < entitiesLength; i++) {
		// get mesh
		Mesh* mesh = entities[i]->GetMesh();
		vertexBuffer = mesh->GetVertexBuffer();

		// set the shaders
		Material* material = entities[i]->GetMaterial();

		// Populate Constant Buffers
		SimpleVertexShader* vs = material->GetVertexShader();
		vs->SetShader();

		vs->SetFloat4("colorTint", material->GetColor());
		vs->SetMatrix4x4("world", entities[i]->GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", camera->GetViewMatrix());
		vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
		vs->SetMatrix4x4("worldInvTrans", entities[i]->GetTransform()->GetWorldInvTrans());
		vs->SetFloat3("cameraPos", camera->GetTransform()->GetPosition());
		vs->SetMatrix4x4("viewInv", camera->GetViewInverse());

		vs->CopyAllBufferData();
		
		SimplePixelShader* ps = material->GetPixelShader();
		ps->SetShader();

		// textures
		ps->SetShaderResourceView("diffuseTexture", material->GetDiffuse());
		// becuase simple shader fails gracefully, can just always set normal
		ps->SetShaderResourceView("normalMap", material->GetNormal());
		ps->SetSamplerState("samplerOptions", material->GetSamplerState());

		ps->SetData("l1",&dl1,sizeof(Light));
		ps->SetData("l2",&dl2,sizeof(Light));
		ps->SetData("l3",&dl3,sizeof(Light));
		ps->SetData("l4",&pl1,sizeof(Light));
		ps->SetFloat("specInt", material->GetSpecInt());

		ps->CopyAllBufferData();

		// set vertex and index buffers
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);


		// draw (!)
		context->DrawIndexed(
			mesh->GetIndexCount(),
			0,
			0);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}