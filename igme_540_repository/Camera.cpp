#include "Camera.h"
using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, float fov, int width, int height, float nearClip, float farClip, float moveSpeed, float rotateSpeed, float mouseRotateSpeed, HWND window)
{
	transform = Transform();
	transform.SetPosition(pos.x, pos.y, pos.z);
	transform.SetRotation(rot.x, rot.y, rot.z);

	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->moveSpeed = moveSpeed;
	this->rotateSpeed = rotateSpeed;
	this->mouseRotateSpeed = mouseRotateSpeed;
	this->mouseCenter = mouseCenter;

	viewMatrix = XMFLOAT4X4();
	dirtyView = true;
	viewInv = XMFLOAT4X4();
	dirtyViewInv = true;
	projectionMatrix = XMFLOAT4X4();
	UpdateProjectionMatrix(width, height, window);
}

Transform* Camera::GetTransform()
{
	return &transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	if (dirtyView) {
		UpdateViewMatrix();
	}
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetViewInverse()
{
	// force update view
	GetViewMatrix();

	// check if inverse needs update
	if (dirtyViewInv) {
		XMStoreFloat4x4(
			&viewInv,
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&viewMatrix))
		);
		dirtyViewInv = false;
	}

	return viewInv;
}

DirectX::XMFLOAT3 Camera::GetVelocity()
{
	return velocity;
}

void Camera::UpdateViewMatrix()
{
	// set vectors
	XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMFLOAT3 pos = transform.GetPosition();

	XMFLOAT4 rot = transform.GetPitchYawRoll();
	XMVECTOR rotV = XMLoadFloat4(&rot);

	// find up and forward
	forward = XMVector3Rotate(forward, rotV);
	up = XMVector3Rotate(up, rotV);

	// create matrix
	XMStoreFloat4x4(&viewMatrix, XMMatrixLookToLH(XMLoadFloat3(&pos), forward, up));
	dirtyView = false;
	dirtyViewInv = true;
}

void Camera::UpdateProjectionMatrix(int width, int height, HWND window)
{
	XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fov, (float)width/(float)height, nearClip, farClip));
	
	RECT r;
	GetWindowRect(window, &r);
	mouseCenter.x = width / 2 + r.left;
	mouseCenter.y = height / 2 + r.top;
}

void Camera::Update(float dt, HWND windowHandle)
{
	float zoomRectifier = 1.f;
	XMVECTOR vector = XMVECTOR();

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		zoomRectifier *= 5;
	}
	if (GetAsyncKeyState(VK_CONTROL) &  0x8000) {
		zoomRectifier *= .2f;
	}

	zoomRectifier *= moveSpeed;

	XMFLOAT3 temp;
	// keyboard movement
	if (GetAsyncKeyState('W') & 0x8000) {
		temp = XMFLOAT3(0, 0, zoomRectifier);
		vector += XMLoadFloat3(&temp);
		dirtyView = true;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		temp = XMFLOAT3(0, 0, -zoomRectifier);
		vector += XMLoadFloat3(&temp);
		dirtyView = true;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		temp = XMFLOAT3(-zoomRectifier, 0, 0);
		vector += XMLoadFloat3(&temp);
		dirtyView = true;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		temp = XMFLOAT3(zoomRectifier, 0, 0);
		vector += XMLoadFloat3(&temp);
		dirtyView = true;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		temp = XMFLOAT3(0, zoomRectifier, 0);
		vector += XMLoadFloat3(&temp);
		dirtyView = true;
	}
	if (GetAsyncKeyState('C') & 0x8000) {
		temp = XMFLOAT3(0, -zoomRectifier, 0);
		vector += XMLoadFloat3(&temp);
		dirtyView = true;
	}

	transform.MoveRelative(dt * vector);
	XMStoreFloat3(&velocity, vector);

	// keyboard rotation
	if (GetAsyncKeyState('Q') & 0x8000) {
		transform.RotateRelative(0, 0, dt * rotateSpeed);
		dirtyView = true;
	}
	if (GetAsyncKeyState('E') & 0x8000) {
		transform.RotateRelative(0, 0, -dt * rotateSpeed);
		dirtyView = true;
	}

	// mouse rotation
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	//ScreenToClient(windowHandle, &mousePos);

	POINT diff = {};
	diff.x = mousePos.x - mouseCenter.x;
	diff.y = mousePos.y - mouseCenter.y;

	transform.RotateRelative(dt * diff.y * mouseRotateSpeed, dt * diff.x * mouseRotateSpeed, 0);
	dirtyView = true;

	SetCursorPos(mouseCenter.x, mouseCenter.y);

	// viewMatrix is updated when dirty
}
