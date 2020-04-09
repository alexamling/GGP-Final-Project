#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <Windows.h>
#include "Transform.h"

class Camera
{
private:
	// fields
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 viewInv;
	DirectX::XMFLOAT4X4 projectionMatrix;
	POINT prevMousePos;
	float fov;
	float nearClip;
	float farClip;
	float moveSpeed;
	float rotateSpeed;
	float mouseRotateSpeed;

	bool dirtyView;
	bool dirtyViewInv;

public:
	// constructors
	Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, float fov, float aspectRatio, float nearClip, float farClip, float moveSpeed, float rotateSpeed, float mouseRotateSpeed);

	// getters
	Transform* GetTransform();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewInverse();

	// methods
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);
	void Update(float dt, HWND windowHandle);
};

