#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects

class Mesh
{
	// Fields
private:
	ID3D11Buffer* vb = nullptr;
	ID3D11Buffer* ib = nullptr;
	int ibLength = 0;

	// Constructors
public:
	Mesh();
	Mesh(Vertex* verts, unsigned int vertsLength, unsigned int* indices, unsigned int indicesLength, Microsoft::WRL::ComPtr<ID3D11Device> device);
	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D11Device> device);

	// Destructors
	~Mesh();

	// Methods
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

private:
	// helpers
	void ConstructBuffers(Vertex* verts, unsigned int vertsLength, unsigned int* indices, unsigned int indicesLength, Microsoft::WRL::ComPtr<ID3D11Device> device);
	D3D11_BUFFER_DESC GetVertexBufferDescription(unsigned int vertsLength);
	D3D11_BUFFER_DESC GetIndexBufferDescription(unsigned int indicesLength);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};

