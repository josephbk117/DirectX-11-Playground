#pragma once
#include "..\Scenegraph And Physics\OBB.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <DirectXCollision.h>

using namespace DirectX;

class Mesh
{
public:
	Mesh(ID3D11Device *device, ID3D11DeviceContext* context, std::vector<Vertex>& vertices, std::vector<DWORD> & indices);
	Mesh(const Mesh& mesh);
	Mesh& operator=(const Mesh & mesh);
	void draw();
	void drawOBB();
	const OBB& getOBB() const
	{
		return obb;
	}
private:
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext * deviceContext = nullptr;
	OBB obb;
};