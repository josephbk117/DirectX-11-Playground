#pragma once
#include "Camera.h"

class Light
{
protected:
	Camera lightCamera;
	bool shadowRenderingEnabled = false;
public:
	float intensity = 1.0f;
	XMFLOAT3 colour = { 1, 0, 1 };
	const XMMATRIX GetLightMatrix() const
	{
		return lightCamera.transform.GetMatrix() * lightCamera.GetProjectionMatrix();
	}

	void setPosition(const XMFLOAT3& pos)
	{
		lightCamera.transform.SetPosition(pos.x, pos.y, pos.z);
	}

	XMFLOAT3 getPosition() const
	{
		return lightCamera.transform.GetPositionFloat3();
	}

	void setRotation(const XMVECTOR& dir)
	{
		lightCamera.transform.SetRotation(dir);
	}

	void enableShadowMapRendering(RenderTexture* shadowMapRendertexture)
	{
#if _DEBUG
		if (shadowMapRendertexture == nullptr)
		{
			ErrorLogger::log("Render texture for shadow map is null");
			exit(-1);
		}
#endif
		shadowRenderingEnabled = true;
		lightCamera.SetRenderTexture(shadowMapRendertexture);
	}

	void disableShadowMapRendering()
	{
		shadowRenderingEnabled = false;
		lightCamera.SetRenderTexture(nullptr);
	}

	bool doesLightRenderShadowMap() const
	{
		return shadowRenderingEnabled;
	}

	void bindShadowMapRenderTexture()const
	{
		lightCamera.GetRenderTexture()->setRenderTarget();
		lightCamera.GetRenderTexture()->clearRenderTarget(1, 1, 1, 1);
	}

	RenderTexture* getShadowMapRenderTexture()const
	{
		return lightCamera.GetRenderTexture();
	}


};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
};

