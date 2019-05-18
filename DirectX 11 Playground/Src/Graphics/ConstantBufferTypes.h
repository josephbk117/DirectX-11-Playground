#pragma once
#include <DirectXMath.h>

struct CB_VS_VertexShader
{
	DirectX::XMMATRIX mvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_VS_Skinned_VertexShader : public CB_VS_VertexShader
{
	DirectX::XMMATRIX jointMatrices[50];
};

struct CB_PS_LightBuffer
{
	float ambientLightIntensity;
	DirectX::XMFLOAT3 ambientLightColour;
};