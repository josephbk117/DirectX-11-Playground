#pragma once
#include <DirectXMath.h>
struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}
	Vertex(const Vertex& rhs) = default;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT2 texCoord = { 0.0f, 0.0f };
	DirectX::XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
};

struct SkinnedVertex : public Vertex
{
	SkinnedVertex() = default;
	SkinnedVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz,
		unsigned int id1, unsigned int id2, unsigned int id3, float weight1, float weight2, float weight3)
		: Vertex(x, y, z, u, v, nx, ny, nz), jointIds{ id1,id2,id3 }, weights{ weight1, weight2, weight3 } {}
	SkinnedVertex(const SkinnedVertex& rhs) = default;
	DirectX::XMUINT3 jointIds = { 0,0,0 };
	DirectX::XMFLOAT3 weights = { 0.0f, 0.0f, 0.0f };
};
