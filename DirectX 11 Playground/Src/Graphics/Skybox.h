#pragma once
#include "Model.h"
#include "CubemapTexture.h"
class Skybox
{
private:
	CubemapTexture cubemap;
	Model model;
public:
	void init(ID3D11Device* device, ID3D11DeviceContext* context, VertexConstantBuffer<CB_VS_VertexShader>& constBuffer, const CubemapTexture& cubemap);
	void draw(const XMMATRIX & viewProjectionMatrix);
};

