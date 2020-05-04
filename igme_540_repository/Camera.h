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
	DirectX::XMFLOAT3 velocity;
	POINT mouseCenter;
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
	Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, float fov, int width, int height, float nearClip, float farClip, float moveSpeed, float rotateSpeed, float mouseRotateSpeed, HWND window);

	// getters
	Transform* GetTransform();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewInverse();
	DirectX::XMFLOAT3 GetVelocity();

	// methods
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(int width, int height, HWND window);
	void Update(float dt, HWND windowHandle);
};

