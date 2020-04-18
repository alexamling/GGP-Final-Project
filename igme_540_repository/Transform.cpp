#include "Transform.h"

Transform::Transform()
{
	position = XMFLOAT3(0,0,0);
	scale = XMFLOAT3(1,1,1);
	XMStoreFloat4(&pitchYawRoll, XMQuaternionIdentity());
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	dirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x,y,z);
	dirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	XMStoreFloat4(&pitchYawRoll, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	dirty = true;
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT4 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty)
	{
		UpdateMatrix();
	}
	return world;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	dirty = true;
}

void Transform::MoveAbsolute(XMVECTOR vector)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + vector);
	dirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	//Get the orientation axis' of the object
	XMVECTOR target = XMVectorSet(x,y,z,0.0f);
	XMVECTOR targetRot = XMLoadFloat4(&pitchYawRoll);
	XMVECTOR dir = XMVector3Rotate(target,targetRot);

	XMStoreFloat3(&position, XMLoadFloat3(&position) + dir);
	dirty = true;
}

void Transform::MoveRelative(XMVECTOR vector)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVector3Rotate(vector, XMLoadFloat4(&pitchYawRoll)));
	dirty = true;
}

void Transform::RotateRelative(float pitch, float yaw, float roll)
{
	// load vectors
	XMVECTOR rotToApply = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR currentRot = XMLoadFloat4(&pitchYawRoll);

	// math (currentRot * currentRot * rotToApply)
	XMStoreFloat4(&pitchYawRoll, XMQuaternionMultiply(rotToApply, currentRot));
	dirty = true;
}

void Transform::RotateRelative(XMVECTOR pitchYawRoll)
{
	XMStoreFloat4(&(this->pitchYawRoll), XMQuaternionMultiply(XMQuaternionRotationRollPitchYawFromVector(pitchYawRoll), XMLoadFloat4(&(this->pitchYawRoll))));
	dirty = true;
}

void Transform::RotateAbsolute(float pitch, float yaw, float roll)
{
	XMStoreFloat4(&pitchYawRoll, XMQuaternionMultiply(XMLoadFloat4(&pitchYawRoll), XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)));
	dirty = true;
}

void Transform::RotateAbsolute(XMVECTOR pitchYawRoll)
{
	XMStoreFloat4(&(this->pitchYawRoll), XMQuaternionMultiply(XMLoadFloat4(&(this->pitchYawRoll)), XMQuaternionRotationRollPitchYawFromVector(pitchYawRoll)));
	dirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= x;
	scale.z *= z;
	dirty = true;
}

void Transform::Scale(XMVECTOR scaleVector)
{
	XMStoreFloat3(&scale, XMVectorMultiply(XMLoadFloat3(&scale), scaleVector));
	dirty = true;
}

void Transform::UpdateMatrix()
{
	//convert position, scaling, and rotation to matricies
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
	XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&pitchYawRoll));

	// Applies translation, then rotation, then scale
	XMMATRIX worldMatrix = scaling * rotation * translation; 
	
	XMStoreFloat4x4(&world,worldMatrix);
	dirty = false;
}
