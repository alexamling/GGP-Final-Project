#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInvTrans, XMMatrixIdentity());
	dirtyWorld = false;
	dirtyWorldInvTrans = false;

	position = XMFLOAT3(0, 0, 0);
	XMStoreFloat4(&rotation, XMQuaternionIdentity());
	scale = XMFLOAT3(1, 1, 1);
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	dirtyWorld = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	dirtyWorld = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return GetEulerAnglesFromQuaternion(rotation);
}

DirectX::XMFLOAT4 Transform::GetRotQuat()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirtyWorld) {
		XMStoreFloat4x4(
			&worldMatrix,
			XMMatrixScaling(scale.x, scale.y, scale.z)
				* XMMatrixRotationQuaternion(XMLoadFloat4(&rotation))
				* XMMatrixTranslation(position.x, position.y, position.z)
		);
		dirtyWorld = false;
		dirtyWorldInvTrans = true;
	}
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInvTrans()
{
	// force update of world
	GetWorldMatrix();

	// check if need update
	if (dirtyWorldInvTrans) {
		XMStoreFloat4x4(
			&worldInvTrans,
			XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix)))
		);
	}

	return worldInvTrans;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
	dirtyWorld = true;
}

void Transform::RotateAbsolute(float pitch, float yaw, float roll)
{
	XMStoreFloat4(&rotation, XMQuaternionMultiply(XMLoadFloat4(&rotation), XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)));
	dirtyWorld = true;
}

void Transform::ScaleAbsolute(float x, float y, float z)
{
	scale = XMFLOAT3(scale.x * x, scale.y * y, scale.z * z);
	dirtyWorld = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	// load vectors
	XMVECTOR transVect = XMVectorSet(x, y, z, 0.f);
	XMVECTOR quat = XMLoadFloat4(&rotation);

	// rotate translation, add to position, then store
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVector3Rotate(transVect, quat));
}

void Transform::RotateRelative(float pitch, float yaw, float roll)
{
	// load vectors
	XMVECTOR rotToApply = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR currentRot = XMLoadFloat4(&rotation);

	// math (currentRot * currentRot * rotToApply)
	XMStoreFloat4(&rotation, XMQuaternionMultiply(rotToApply, currentRot));
}

void Transform::ScaleRelative(float x, float y, float z)
{
	// load Vectors
	XMFLOAT3 scaleToApply = XMFLOAT3(x, y, z);
	XMVECTOR scaleVector = XMLoadFloat3(&scaleToApply);

	// rotate Scaling
	scaleVector = XMVector3Rotate(scaleVector, XMLoadFloat4(&rotation));

	// convert back to float
	XMStoreFloat3(&scaleToApply, scaleVector);

	// save applied scale
	scale = XMFLOAT3(scale.x * scaleToApply.x, scale.y * scaleToApply.y, scale.z * scaleToApply.z);
}

DirectX::XMFLOAT3 Transform::GetEulerAnglesFromQuaternion(DirectX::XMFLOAT4 quat)
{
	return XMFLOAT3(	// thanks, i hate it
		atan2f(2 * (rotation.w * rotation.x + rotation.y * rotation.z), 1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y)),
		asinf(2 * (rotation.w * rotation.y - rotation.z * rotation.x)),
		atan2f(2 * (rotation.w * rotation.z + rotation.x, rotation.y), 1 - 2 * (rotation.y * rotation.y + rotation.z * rotation.z))
	);
}
