#pragma once
#include <DirectXMath.h>

struct CB_VS_VertexShader
{
	DirectX::XMMATRIX mvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};