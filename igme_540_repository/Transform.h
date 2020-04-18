#pragma once
#include "DXCore.h"
#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	//Constructinator
	Transform();

	//Setters
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	//Getters
	XMFLOAT3 GetPosition();
	XMFLOAT4 GetPitchYawRoll();
	XMFLOAT3 GetScale();
	XMFLOAT4X4 GetWorldMatrix();

	//Movement functions
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(XMVECTOR vector);
	void MoveRelative(float x, float y, float z);
	void MoveRelative(XMVECTOR vector);
	void RotateRelative(float pitch, float yaw, float roll);
	void RotateRelative(XMVECTOR pitchYawRoll);
	void RotateAbsolute(float pitch, float yaw, float roll);
	void RotateAbsolute(XMVECTOR pitchYawRoll);
	void Scale(float x, float y, float z);
	void Scale(XMVECTOR scaleVector);

private:
	//Raw variable data
	XMFLOAT4X4 world;
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT4 pitchYawRoll;

	//bool for updating matrix
	bool dirty = false;

	//Update the world matrix
	void UpdateMatrix();
};

