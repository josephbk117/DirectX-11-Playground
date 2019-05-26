#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace DirectX;

class Mesh
{
public:
	Mesh(ID3D11Device *device, ID3D11DeviceContext* context, std::vector<Vertex>& vertices, std::vector<DWORD> & indices);
	Mesh(const Mesh& mesh);
	Mesh& operator=(const Mesh & mesh);
	void draw();
private:
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext * deviceContext = nullptr;
};