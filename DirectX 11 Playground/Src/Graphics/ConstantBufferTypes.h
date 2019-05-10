#pragma once
#include <DirectXMath.h>

struct CB_VS_VertexShader
{
	DirectX::XMMATRIX mvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_PS_LightBuffer
{
	float ambientLightIntensity;
	DirectX::XMFLOAT3 ambientLightColour;
};