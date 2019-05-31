#include "Camera.h"

Camera::Camera()
{
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

void Camera::SetRenderTexture(RenderTexture * renderTexture)
{
	this->renderTexture = renderTexture;
}

RenderTexture * Camera::GetRenderTexture() const
{
	return renderTexture;
}

bool Camera::HasRenderTexture() const
{
	return renderTexture != nullptr;
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