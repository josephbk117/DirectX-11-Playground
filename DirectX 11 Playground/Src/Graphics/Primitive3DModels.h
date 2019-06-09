#pragma once
#include "Vertex.h"
#include <vector>
#include <d3d11.h>

namespace Primitive3DModels
{
	struct VerticesIndicesPair
	{
		std::vector<Vertex> vertices;
		std::vector<DWORD> indices;
	};

	const VerticesIndicesPair QUAD
	{
		{
			Vertex{-0.5f,0.5f,0,0,0,        0,0,-1.0f},
			Vertex{0.5f,0.5f,0,1.0f,0,      0,0,-1.0f},
			Vertex{-0.5f,-0.5f,0,0,1.0f,    0,0,-1.0f},
			Vertex{0.5f,-0.5f,0,1.0f,1.0f,  0,0,-1.0f},
		},
		{
			0, 1, 2,
			2, 1, 3
		}
	};
}