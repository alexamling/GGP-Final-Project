#include "RegularGenerator.h"

void RegularGenerator::Generate(int sides, XMFLOAT3 positionOffset, float scale,
	Vertex*& vertArray, unsigned int& vertArrayLength, unsigned int*& indexArray, unsigned int& indexArrayLength)
{
	// side check
	if (sides < 3)
	{
		return;
	}

	// create Arrays
	vertArrayLength = sides;
	vertArray = new Vertex[vertArrayLength];
	indexArrayLength = (sides - 2) * 3;
	indexArray = new unsigned int[indexArrayLength];

	float deltaTheta = XM_2PI / sides;

	// Create Vertices
	for (int i = 0; i < sides; i++)
	{
		vertArray[i].position = XMFLOAT3(cosf(deltaTheta * i) * scale + positionOffset.x, sinf(deltaTheta * i) * scale + positionOffset.y, positionOffset.z);
		vertArray[i].normal = XMFLOAT3(0, 0, -1);
		vertArray[i].uv = XMFLOAT2(0, 0);
	}

	// Create triangle fan in index array
	for (int i = 0; i < sides - 2; i++)
	{
		indexArray[i * 3] = 0;			// Anchor point
		indexArray[i * 3 + 1] = i + 2;	// point 2
		indexArray[i * 3 + 2] = i + 1;	// point 1
	}
}
