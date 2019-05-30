#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

void Material::setRenderStates(ID3D11DepthStencilState* depthStencilState, ID3D11RasterizerState* rasterizerState, ID3D11SamplerState* samplerState, ID3D11BlendState* blendState)
{
	this->depthStencilState = depthStencilState;
	this->rasterizerState = rasterizerState;
	this->samplerState = samplerState;
	this->blendState = blendState;

#if _DEBUG
	if (this->depthStencilState != nullptr && this->rasterizerState != nullptr && this->samplerState != nullptr && this->blendState != nullptr)
		isCompletlyInitialized[0] = true;
#endif
}

void Material::setShadowAndRenderQueueStates(RenderQueue queue, bool castShadow, bool recieveShadow)
{
	this->renderQueue = queue;
	this->castShadow = castShadow;
	this->recieveShadow = recieveShadow;
}

void Material::setRenderQueue(RenderQueue queue)
{
	this->renderQueue = queue;
}

void Material::setIfCastsShadow(bool castShadow)
{
	this->castShadow = castShadow;
}

void Material::setIfRecieveShadow(bool recieveShadow)
{
	this->recieveShadow = recieveShadow;
}

void Material::setShaders(VertexShader* vertexShader, PixelShader* pixelShader)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;

#if _DEBUG
	if (this->vertexShader != nullptr && this->pixelShader != nullptr)
		isCompletlyInitialized[1] = true;
#endif
}

void Material::setTopologyType(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	this->topology = topology;
}

void Material::addVertexConstantBuffer(BaseVertexConstantBuffer* constantBuffer)
{
	vertexConstantBuffers.push_back(constantBuffer);

#if _DEBUG
	isCompletlyInitialized[2] = true;
	for (BaseConstantBuffer* bcb : vertexConstantBuffers)
	{
		if (bcb == nullptr)
		{
			isCompletlyInitialized[2] = false;
			break;
		}
	}
#endif
}

void Material::addPixelConstantBuffer(BasePixelConstantBuffer* constantBuffer)
{
	pixelConstantBuffers.push_back(constantBuffer);

#if _DEBUG
	isCompletlyInitialized[3] = true;
	for (BaseConstantBuffer* bcb : pixelConstantBuffers)
	{
		if (bcb == nullptr)
		{
			isCompletlyInitialized[2] = false;
			break;
		}
	}
#endif
}

bool Material::doesCastShadow() const
{
	return castShadow;
}

bool Material::doesRecieveShadow() const
{
	return recieveShadow;
}

RenderQueue Material::getRenderQueue() const
{
	return renderQueue;
}

void Material::getShadowAndRenderQueueStates(RenderQueue & queue, bool & castShadow, bool & recieveShadow)
{
	queue = this->renderQueue;
	castShadow = this->castShadow;
	recieveShadow = this->recieveShadow;
}

void Material::bind(ID3D11DeviceContext * context) const
{
#if _DEBUG
	if (std::find(&isCompletlyInitialized[0], &isCompletlyInitialized[3], false) == nullptr)
	{
		ErrorLogger::log("Material was not completly initialized before binding");
		exit(-1);
	}
#endif

	context->IASetInputLayout(vertexShader->getInputLayout());
	context->IASetPrimitiveTopology(topology);
	context->RSSetState(rasterizerState);
	context->OMSetDepthStencilState(depthStencilState, 0);
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFF);
	context->PSSetSamplers(0, 1, &samplerState);
	context->VSSetShader(vertexShader->getShader(), NULL, 0);
	context->PSSetShader(pixelShader->getShader(), NULL, 0);

	for (unsigned int index = 0; index < vertexConstantBuffers.size(); index++)
		context->VSSetConstantBuffers(index, 1, vertexConstantBuffers[index]->getAddressOf());

	for (unsigned int index = 0; index < pixelConstantBuffers.size(); index++)
		context->PSSetConstantBuffers(index, 1, pixelConstantBuffers[index]->getAddressOf());
}
