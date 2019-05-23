#include "Skybox.h"
#include "Primitive3DModels.h"
void Skybox::init(ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<CB_VS_VertexShader>& constBuffer, const CubemapTexture & cubemap)
{
	this->cubemap = cubemap;
	model.init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device, context, this->cubemap.getTextures()[0].Get(), constBuffer);
}

void Skybox::draw(const XMMATRIX & viewProjectionMatrix)
{
	model.setTexture(cubemap.getTextures()[0].Get());
	model.draw(XMMatrixTranslation(0, 0, 0.5f)*XMMatrixScaling(100, 100, 100)*viewProjectionMatrix);

	model.setTexture(cubemap.getTextures()[1].Get());
	model.draw(XMMatrixRotationY(3.141f)*XMMatrixTranslation(0, 0, -0.5f)*XMMatrixScaling(100, 100, 100)*viewProjectionMatrix);

	model.setTexture(cubemap.getTextures()[2].Get());
	model.draw(XMMatrixRotationY(3.141f / 2.0f)*XMMatrixTranslation(0.5f, 0, 0)*XMMatrixScaling(100, 100, 100)*viewProjectionMatrix);

	model.setTexture(cubemap.getTextures()[3].Get());
	model.draw(XMMatrixRotationY(-3.141f / 2.0f)*XMMatrixTranslation(-0.5f, 0, 0)*XMMatrixScaling(100, 100, 100)*viewProjectionMatrix);

	model.setTexture(cubemap.getTextures()[4].Get());
	model.draw(XMMatrixRotationX(3.141f / 2.0f)*XMMatrixTranslation(0, -0.5f, 0)*XMMatrixScaling(100, 100, 100)*viewProjectionMatrix);

	model.setTexture(cubemap.getTextures()[5].Get());
	model.draw(XMMatrixRotationX(-3.141f / 2.0f)*XMMatrixTranslation(0, 0.5f, 0)*XMMatrixScaling(100, 100, 100)*viewProjectionMatrix);
}
