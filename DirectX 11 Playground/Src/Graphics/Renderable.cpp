#include "Renderable.h"

Renderable::Renderable()
{
}

Renderable::Renderable(Material * mat, Model * model)
{
	this->m_material = mat;
	this->m_model = model;
}

Renderable::~Renderable()
{
	//delete model;
}

Material * Renderable::getMaterial() const
{
	return m_material;
}

ModelInterface * Renderable::getModel() const
{
	return m_model;
}

void Renderable::draw(ID3D11DeviceContext* context, const XMMATRIX& viewProjectionMatrix)
{
	m_material->bind(context);
	m_model->draw(transform.GetMatrix(), viewProjectionMatrix);
}

void Renderable::overrideMaterialDraw(ID3D11DeviceContext * context, Material * material, const XMMATRIX & viewProjectionMatrix)
{
	material->bind(context);
	m_model->draw(transform.GetMatrix(), viewProjectionMatrix);
}

void Renderable::overrideWorldMatrixDraw(ID3D11DeviceContext * context, const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	m_material->bind(context);
	m_model->draw(worldMatrix, viewProjectionMatrix);
}

void Renderable::overrideWorldMatrixAndMaterial(ID3D11DeviceContext * context, Material * material, const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	material->bind(context);
	m_model->draw(worldMatrix, viewProjectionMatrix);
}

void Renderable::setShadowMapTexture(RenderTexture* texture)
{
	m_model->setTexture2(texture->getShaderResourceView());
}
