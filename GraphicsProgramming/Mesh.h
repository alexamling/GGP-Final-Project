#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include <fstream>
#include <vector>

using namespace DirectX;

class Mesh
{
public:
	Mesh(Vertex* vertexArray, int vertexCount, UINT* indexArray, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D11Device> device);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	int GetIndexCount();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	int indexCount;

	void CreateBuffers(Vertex* vertexArray, int vertexCount, UINT* indexArray, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
};

