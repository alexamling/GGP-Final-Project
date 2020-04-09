#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <iostream>
#include <fstream>
#include <vector>

class Mesh
{
public:
	Mesh(Vertex* points,int vertCount, UINT* indices, int indicesCount, 
		Microsoft::WRL::ComPtr<ID3D11Device> object, Microsoft::WRL::ComPtr<ID3D11DeviceContext>objConx);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device> object,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> objConX, const char* filePath);

	void DrawMesh();
	void SetBuffers();
	int GetIndexCount();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	
	void CreateVertexBuffer();
	void CreateIndexBuffer();

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	
	int GetVertexCount();

	
	Microsoft::WRL::ComPtr<ID3D11Buffer> verBuff;	//Vertex Buffer Smart Pointer
	Microsoft::WRL::ComPtr<ID3D11Buffer> indBuff;	//Index Buffer Smart Pointer
	Microsoft::WRL::ComPtr<ID3D11Device> device;	//D3D smart pointer
													//DeviceContex smart pointer
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;	

	Vertex* vertices;								//Array of vertices
	UINT* indxArr;									//Array of indices

	int indexCount;									//Indices count
	int vertexCount;								//Vertex count
};

