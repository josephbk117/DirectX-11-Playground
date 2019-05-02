#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

using namespace DirectX;

class Model
{
public:
	bool init(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader);
	void setTexture(ID3D11ShaderResourceView* texture);
	void draw(const XMMATRIX& viewProjectionMatrix);
private:
	void updateWorldMatrix();
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ConstantBuffer<CB_VS_VertexShader> * cb_vs_vertexShader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;

	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

