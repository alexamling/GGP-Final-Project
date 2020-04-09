#pragma once
#include "DXCore.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"

using namespace DirectX;

// Fun static class that can generate regular polygons and add colors to them
class RegularGenerator
{
	// Methods
public:
	static void Generate(int sides, XMFLOAT3 positionOffset, float scale, Vertex*& vertArray, unsigned int& vertArrayLength, unsigned int*& indexArray, unsigned int& indexArrayLength);
};

