#pragma once
#include <Windows.h>
#include "Transform.h"

class Camera
{
public:
	Camera(Transform t, float aspectRatio, float FOV, float nearClip, float farClip, float moveSpeed, float mouseLookSpeed);

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt, HWND windowHandle);
	DirectX::XMFLOAT3 GetPos();
private:
	Transform transform;
	
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	
	POINT previousMousePos;
	
	float FOV;

	float nearClip;
	float farClip;
	float moveSpeed;
	float mouseLookSpeed;
};

