#pragma once
#include "Material.h"
#include "Model.h"
#include "..\Scenegraph And Physics\Transform.h"
class Renderable
{
private:
	Material* material = nullptr;
	Model* model = nullptr;
public:
	Renderable();
	Renderable(Material* mat, Model* model);
	~Renderable();
	Material* getMaterial() const;
	Model* getModel() const;
	Transform transform;
};