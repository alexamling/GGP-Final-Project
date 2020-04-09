#pragma once

#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT3 DiffuseColor;
	float padding1;
	DirectX::XMFLOAT3 Direction;
	float Intensity;
};

