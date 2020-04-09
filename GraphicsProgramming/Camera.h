#pragma once
#include <Windows.h>
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
public:
	Camera(XMVECTOR position, XMVECTOR orientation, float aspectRatio, float field, float nClip, float fClip, float move, float look);
	~Camera();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjMatrix();
	Transform* GetTransform();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt, HWND windowHandle);

private:
	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;

	POINT prevMousePos;

	Transform* trans;

	float FoV;
	float nearClip = .01f;
	float farClip = 1000.0f;
	float moveSpeed;
	float lookSpeed;

};

