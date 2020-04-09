#pragma once

#include <DirectXMath.h>

class Transform
{
	// fields
private:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInvTrans;
	bool dirtyWorld;
	bool dirtyWorldInvTrans;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 scale;

	// Constructor
public:
	Transform();

	// methods
public:
	// Setters
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	// Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT4 GetRotQuat();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInvTrans();

	// Transformers
	void MoveAbsolute(float x, float y, float z);
	void RotateAbsolute(float pitch, float yaw, float roll);
	void ScaleAbsolute(float x, float y, float z);

	void MoveRelative(float x, float y, float z);
	void RotateRelative(float pitch, float yaw, float roll);
	void ScaleRelative(float x, float y, float z);

	// helper functions
private:
	DirectX::XMFLOAT3 GetEulerAnglesFromQuaternion(DirectX::XMFLOAT4 quat);
};

