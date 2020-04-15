#pragma once
#include "DXCore.h"
#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetPitchYawRoll();
	XMFLOAT3 GetScale();
	XMFLOAT4X4 GetWorldMatrix();

	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);

	void UpdateMatrix();

private:
	//Raw variable data
	XMFLOAT4X4 world;
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 pitchYawRoll;
};

