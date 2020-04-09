#pragma once
#include <DirectXMath.h>

struct PointLight
{
	DirectX::XMFLOAT3 Position;
	float padding1;
	DirectX::XMFLOAT3 Color;
	float Intensity;
	float Range;
};