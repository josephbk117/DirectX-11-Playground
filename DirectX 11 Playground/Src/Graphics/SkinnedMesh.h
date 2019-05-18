#pragma once
#include "Mesh.h"
class SkinnedMesh
{
public:
	SkinnedMesh(ID3D11Device *device, ID3D11DeviceContext* context, std::vector<SkinnedVertex>& vertices, std::vector<DWORD> & indices);
	SkinnedMesh(const SkinnedMesh& mesh);
	void draw();
private:
	VertexBuffer<SkinnedVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext * deviceContext;
};

