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
			Vertex{-1,1,0,0,0,0,0,1},
			Vertex{1,1,0,1,0,0,0,1},
			Vertex{-1,-1,0,0,1,0,0,1},
			Vertex{1,-1,0,1,1,0,0,1},
		},
		{
			0, 1, 2,
			2, 1, 3
		}
	};
}