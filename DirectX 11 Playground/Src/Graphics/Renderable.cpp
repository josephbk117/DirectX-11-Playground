#include "Renderable.h"

Renderable::Renderable()
{
}

Renderable::Renderable(Material * mat, Model * model)
{
	this->material = mat;
	this->model = model;
}

Renderable::~Renderable()
{
}

Material * Renderable::getMaterial() const
{
	return material;
}

Model * Renderable::getModel() const
{
	return model;
}
