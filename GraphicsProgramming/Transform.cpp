#include "Transform.h"

Transform::Transform()
{
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
	SetScale(1, 1, 1);
	upToDate = true;
}

void Transform::SetPosition(float x, float y, float z)
{
	upToDate = false;
	position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	upToDate = false;
	rotation = DirectX::XMFLOAT3(pitch, yaw, roll);
}

void Transform::SetScale(float x, float y, float z)
{
	upToDate = false;
	scale = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return rotation;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (upToDate) return worldMatrix;

	DirectX::XMMATRIX positionMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	DirectX::XMMATRIX world = scaleMatrix * rotationMatrix * positionMatrix;

	DirectX::XMStoreFloat4x4(&worldMatrix, world);
	upToDate = false;
	return worldMatrix;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	upToDate = false;
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::MoveRelative(float x, float y, float z)
{
	// pretty version
	/*
	DirectX::XMVECTOR movementVector = DirectX::XMVectorSet(x, y, z, 0);
	DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	movementVector = DirectX::XMVector3Rotate(movementVector, rotationQuat);
	DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);
	DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(movementVector, positionVector));
	*/

	// memory effecient version
	DirectX::XMStoreFloat3(
		&position, 
		DirectX::XMVectorAdd(
			DirectX::XMVector3Rotate(DirectX::XMVectorSet(x, y, z, 0), DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)),
			DirectX::XMLoadFloat3(&position)));
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	upToDate = false;
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
}

void Transform::Scale(float x, float y, float z)
{
	upToDate = false;
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}