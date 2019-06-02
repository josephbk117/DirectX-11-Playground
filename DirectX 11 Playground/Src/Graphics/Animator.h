#pragma once
#include "Renderable.h"
#include <DirectXMath.h>
class Renderable;
class Animator
{
public:
	static void animate(float time, XMMATRIX* jointMatrices, Renderable* renderable);
};

