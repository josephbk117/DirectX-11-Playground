#include "Camera.h"

Camera::Camera()
{
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
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
	XMFLOAT3 pos = GetPositionFloat3();
	XMMATRIX temp = XMMatrixTranslation(pos.x, pos.y, pos.z) * GetMatrix();
	return temp;
}

const XMMATRIX & Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

void Camera::UpdateMatrix()
{
	//Calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the camera's current position
	camTarget += this->posVector;
	//Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	this->matrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}