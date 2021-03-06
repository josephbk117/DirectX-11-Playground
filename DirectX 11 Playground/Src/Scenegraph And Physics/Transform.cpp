#include "Transform.h"

BaseTransform::BaseTransform()
{
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
	SetScale(1, 1, 1);
}

const XMMATRIX & BaseTransform::GetMatrix() const
{
	return this->matrix;
}

const XMVECTOR & BaseTransform::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & BaseTransform::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & BaseTransform::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & BaseTransform::GetRotationFloat3() const
{
	return this->rot;
}

void BaseTransform::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}

void BaseTransform::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void BaseTransform::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}

void BaseTransform::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void BaseTransform::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	this->rotVector = XMVector3Normalize(this->rotVector);
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}

void BaseTransform::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->rotVector = XMVector3Normalize(this->rotVector);
	this->UpdateMatrix();
}

void BaseTransform::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}

void BaseTransform::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void BaseTransform::SetScale(const XMVECTOR& scale)
{
	XMStoreFloat3(&this->scale, scale);
	this->scaleVector = scale;
	this->UpdateMatrix();
}

void BaseTransform::SetScale(float x, float y, float z)
{
	this->scale = XMFLOAT3(x, y, z);
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}

void BaseTransform::AdjustScale(const XMVECTOR& scale)
{
	this->scaleVector += scale;
	XMStoreFloat3(&this->scale, this->scaleVector);
	this->UpdateMatrix();
}

void BaseTransform::AdjustScale(float x, float y, float z)
{
	this->scale.x += x;
	this->scale.y += y;
	this->scale.z += z;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}

void BaseTransform::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrtf(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atanf(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atanf(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR & BaseTransform::GetForwardVector() const
{
	return this->vec_forward;
}

const XMVECTOR & BaseTransform::GetRightVector() const
{
	return this->vec_right;
}

const XMVECTOR & BaseTransform::GetBackwardVector() const
{
	return this->vec_backward;
}

const XMVECTOR & BaseTransform::GetLeftVector() const
{
	return this->vec_left;
}

void BaseTransform::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	this->matrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z ) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
}

void Transform::UpdateMatrix()
{
	this->matrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z));
}
