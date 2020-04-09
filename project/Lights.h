#pragma once
#include "DXCore.h"
#include <DirectXMath.h>

struct Light {
	int type;
	DirectX::XMFLOAT3 ambientColor;
	DirectX::XMFLOAT3 diffuseColor;
	float radius;
	DirectX::XMFLOAT3 direction;
};