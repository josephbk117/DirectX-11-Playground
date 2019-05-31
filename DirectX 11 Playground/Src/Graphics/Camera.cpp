#include "Camera.h"

Camera::Camera()
{
	/*this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();*/
	transform.SetPosition(0, 0, 0);
	transform.SetRotation(0, 0, 0);
}

void Camera::SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	projectionType = ProjectionType::PERSPECTIVE;
}

void Camera::SetOrthographicProjectionValues(float width, float height, float nearZ, float farZ)
{
	this->projectionMatrix = XMMatrixOrthographicLH(width, height, nearZ, farZ);
	projectionType = ProjectionType::OTHOGRAPHIC;
}

const Camera::ProjectionType Camera::GetProjectionType() const
{
	return projectionType;
}

const XMMATRIX Camera::GetViewDirectionMatrix() const
{
	XMFLOAT3 pos = transform.GetPositionFloat3();
	XMMATRIX temp = XMMatrixTranslation(pos.x, pos.y, pos.z) * transform.GetMatrix();
	return temp;
}

const XMMATRIX & Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}