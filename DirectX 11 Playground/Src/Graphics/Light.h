#pragma once
#include "Camera.h"

class Light
{
protected:
	Camera lightCamera;
public:
	float intensity = 1.0f;
	XMFLOAT3 colour;
	const XMMATRIX GetLightMatrix() const
	{
		return lightCamera.GetViewMatrix() * lightCamera.GetProjectionMatrix();
	}
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
};

