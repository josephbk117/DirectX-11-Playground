#pragma once
#include <DirectXMath.h>
struct Vertex
{
private:

public:
	Vertex();
	Vertex(float x, float y) :pos(x, y) {}
	DirectX::XMFLOAT2 pos;
};

