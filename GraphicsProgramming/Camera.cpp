#include "Camera.h"

Camera::Camera(Transform t, float aspectRatio, float fov, float nearC, float farC, float moveS, float mouseS)
{
	transform = t;
	nearClip = nearC;
	farClip = farC;
	moveSpeed = moveS;
	mouseLookSpeed = mouseS;

	FOV = fov;
	previousMousePos = {};
	projectionMatrix = {};
	viewMatrix = {};

	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(FOV, aspectRatio, nearClip, farClip));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationRollPitchYaw(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z);
	direction = DirectX::XMVector3Rotate(direction, rotationQuat);

	DirectX::XMFLOAT3 pos = transform.GetPosition();
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0), direction, DirectX::XMVectorSet(0, 1, 0, 0)));
}

void Camera::Update(float dt, HWND windowHandle)
{
	// TODO: Move to an input manager
	// keyboard input
	float s = moveSpeed * dt;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) { s *= 1.5f; }

	if (GetAsyncKeyState('W') & 0x8000)      { transform.MoveRelative( 0, 0, s); }
	if (GetAsyncKeyState('A') & 0x8000)      { transform.MoveRelative(-s, 0, 0); }
	if (GetAsyncKeyState('S') & 0x8000)      { transform.MoveRelative( 0, 0,-s); }
	if (GetAsyncKeyState('D') & 0x8000)      { transform.MoveRelative( s, 0, 0); }
	if (GetAsyncKeyState('X') & 0x8000)      { transform.MoveRelative( 0,-s, 0); }
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { transform.MoveRelative( 0, s, 0); }

	// mouse input
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		float dX = (float)mousePos.x - previousMousePos.x;
		float dY = (float)mousePos.y - previousMousePos.y;
		dX *= dt * mouseLookSpeed;
		dY *= dt * mouseLookSpeed;
		transform.Rotate(dY, dX, 0);
	}

	previousMousePos = mousePos;
	UpdateViewMatrix();

}

DirectX::XMFLOAT3 Camera::GetPos()
{
	return transform.GetPosition();
}
