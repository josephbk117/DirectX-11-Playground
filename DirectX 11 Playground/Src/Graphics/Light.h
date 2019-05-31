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
		return lightCamera.transform.GetMatrix() * lightCamera.GetProjectionMatrix();
	}

	void setPosition(const XMFLOAT3& pos)
	{
		lightCamera.transform.SetPosition(pos.x, pos.y, pos.z);
	}

	XMFLOAT3 getPosition()
	{
		return lightCamera.transform.GetPositionFloat3();
	}

	void setRotation(const XMVECTOR& dir)
	{
		lightCamera.transform.SetRotation(dir);
	}
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
};

