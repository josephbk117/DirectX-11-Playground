#pragma once
#include "Camera.h"

class Light
{
protected:
	Camera lightCamera;
public:
	float intensity = 1.0f;
	XMFLOAT3 colour = { 1,0,1 };
	const XMMATRIX GetLightMatrix() const
	{
		return lightCamera.GetViewMatrix() * lightCamera.GetProjectionMatrix();
	}

	void setPosition(const XMFLOAT3& pos)
	{
		lightCamera.SetPosition(pos.x, pos.y, pos.z);
	}

	XMFLOAT3 getPosition()
	{
		return lightCamera.GetPositionFloat3();
	}
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
};

