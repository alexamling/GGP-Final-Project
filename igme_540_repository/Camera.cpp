#include "Camera.h"

Camera::Camera(XMVECTOR position, XMVECTOR orientation, float aspectRatio, 
	float fov, float nClip, float fClip, float move, float look)
{
	//Camera transform
	trans = new Transform();
	
	//Position
	XMFLOAT3 pos;
	XMStoreFloat3(&pos,position);
	trans->SetPosition(pos.x, pos.y, pos.z);

	//Rotation
	XMFLOAT3 ori;
	XMStoreFloat3(&ori, orientation);
	trans->SetRotation(ori.x,ori.y,ori.z);

	//Camera render specifications
	FoV = fov;
	nearClip = nClip;
	farClip = fClip;
	
	//Camera speeds
	moveSpeed = move;
	lookSpeed = look;
	
	//Mouse Positions
	prevMousePos.x = 0;
	prevMousePos.y = 0;

	//Matrix updates
	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();

}

Camera::~Camera()
{
	delete trans;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return view;
}

XMFLOAT4X4 Camera::GetProjMatrix()
{
	return proj;
}

Transform* Camera::GetTransform()
{
	return trans;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(FoV,aspectRatio,nearClip,farClip));
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT4 rotation = trans->GetPitchYawRoll();
	XMVECTOR Rot = XMLoadFloat4(&rotation);
	
	XMVECTOR worldForward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR newForward = XMVector3Rotate(worldForward, Rot);\

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	up = XMVector3Rotate(up, Rot);

	XMStoreFloat4x4(&view,XMMatrixLookToLH(XMLoadFloat3(&trans->GetPosition()),
		newForward, up));
}

void Camera::Update(float dt, HWND windowHandle)
{
	float speedScale = dt;

#pragma region Speed Scaling
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		/*Speed up Movement Speed*/ 
		speedScale *= 1.8f;
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		/*Slow down Movement Speed*/ 
		speedScale *= 0.6f;
	}
#pragma endregion

#pragma region RelativeMovement
	if (GetAsyncKeyState('W') & 0x8000) {
		/*Relative forward*/
		trans->MoveRelative(0, 0, moveSpeed * speedScale);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		/*Relative Backward*/ 
		trans->MoveRelative(0, 0, moveSpeed * speedScale * -1);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		/*Relative Left*/
		trans->MoveRelative(moveSpeed * speedScale * -1, 0, 0);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		/*Relative Right*/
		trans->MoveRelative(moveSpeed * speedScale, 0, 0);
	}
#pragma endregion

#pragma region AbsoluteMovement
	if (GetAsyncKeyState('E') & 0x8000) {
		/*Absolute Up*/
		trans->MoveAbsolute(0, moveSpeed * speedScale, 0);
	}
	if (GetAsyncKeyState('Q') & 0x8000) {
		/*Absolute Down*/
		trans->MoveAbsolute(0, moveSpeed * speedScale * -1, 0);
	}
#pragma endregion

#pragma region MouseRotation
	//Get mouse position
	POINT mousePos = {}; 
	GetCursorPos(&mousePos); 
	ScreenToClient(windowHandle, &mousePos);

	float deltaY = (mousePos.x - prevMousePos.x) * dt * lookSpeed;
	float deltaX = (mousePos.y - prevMousePos.y) * dt * lookSpeed;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		/*Aim camera*/ 
		trans->RotateRelative(deltaX,deltaY,0);
	}

	prevMousePos.x = mousePos.x;
	prevMousePos.y = mousePos.y;
#pragma endregion

	UpdateViewMatrix();

}
