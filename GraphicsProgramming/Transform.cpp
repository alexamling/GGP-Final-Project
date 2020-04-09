#include "Transform.h"

Transform::Transform()
{
	position = XMFLOAT3(0,0,0);
	scale = XMFLOAT3(1,1,1);
	pitchYawRoll = XMFLOAT3(0, 0, 0);

	UpdateMatrix();
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x,y,z);
	UpdateMatrix();
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll = XMFLOAT3(pitch, yaw, roll);
	UpdateMatrix();
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	UpdateMatrix();
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return world;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	UpdateMatrix();
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR target = XMVectorSet(x,y,z,0);

	XMVECTOR targetRot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	XMVECTOR dir = XMVector3Rotate(target,targetRot);

	XMStoreFloat3(&position, XMLoadFloat3(&position) + dir);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
	UpdateMatrix();
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= x;
	scale.z *= z;
	UpdateMatrix();
}

void Transform::UpdateMatrix()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
	XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	//XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat3(&pitchYawRoll));

	// Applies translation, then rotation, then scale
	XMMATRIX worldMatrix = scaling * rotation * translation; 
	
	XMStoreFloat4x4(&world,worldMatrix);
}
