#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace DirectX;

struct OBB
{
	XMFLOAT3 negExtent;
	XMFLOAT3 posExtent;
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	void draw(ID3D11DeviceContext* context)
	{
		UINT offset = 0;
		context->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
		context->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
	}
};

class Mesh
{
public:
	Mesh(ID3D11Device *device, ID3D11DeviceContext* context, std::vector<Vertex>& vertices, std::vector<DWORD> & indices);
	Mesh(const Mesh& mesh);
	Mesh& operator=(const Mesh & mesh);
	void draw();
	void drawOBB();
private:
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext * deviceContext = nullptr;
	OBB obb;
};