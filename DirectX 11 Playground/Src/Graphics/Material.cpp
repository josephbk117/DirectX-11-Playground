#include "Material.h"

Material* Material::prevBoundMaterial = nullptr;
ID3D11DepthStencilState* Material::prevdepthStencilState = nullptr;
ID3D11RasterizerState* Material::prevRasterizerState = nullptr;
ID3D11SamplerState* Material::prevSamplerState = nullptr;
ID3D11BlendState* Material::prevBlendState = nullptr;
VertexShader* Material::prevVertexShader = nullptr;
PixelShader* Material::prevPixelShader = nullptr;

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
	this->samplerState1 = samplerState;
	this->blendState = blendState;

#if _DEBUG
	if (this->depthStencilState != nullptr && this->rasterizerState != nullptr && this->samplerState1 != nullptr && this->blendState != nullptr)
		isCompletlyInitialized[0] = true;
#endif
}

void Material::setSamplerState2(ID3D11SamplerState* samplerState)
{
	this->samplerState2 = samplerState;
}

void Material::setSamplerState3(ID3D11SamplerState* samplerState)
{
	this->samplerState3 = samplerState;
}

void Material::setShadowAndRenderQueueStates(int queueIndex, bool castShadow, bool recieveShadow)
{
	this->renderQueue = queueIndex;
	this->castShadow = castShadow;
	this->recieveShadow = recieveShadow;
}

void Material::setRenderQueue(int queueIndex)
{
	this->renderQueue = queueIndex;
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
	vertexCBdataAndSizePairs.push_back(std::make_pair<void*, unsigned int>(nullptr, 0));
}

void Material::addPixelConstantBuffer(BasePixelConstantBuffer* constantBuffer)
{
	pixelConstantBuffers.push_back(constantBuffer);
	pixelCBdataAndSizePairs.push_back(std::make_pair<void*, unsigned int>(nullptr, 0));
}

void Material::setVertexConstantBufferData(unsigned int index, void* data, unsigned int dataSize)
{
	if (vertexCBdataAndSizePairs.at(index).first != nullptr)
		delete vertexCBdataAndSizePairs.at(index).first;

	vertexCBdataAndSizePairs.at(index).first = new char[dataSize];
	std::memcpy(vertexCBdataAndSizePairs.at(index).first, data, dataSize);
	vertexCBdataAndSizePairs.at(index).second = dataSize;
}

void Material::setPixelConstantBufferData(unsigned int index, void* data, unsigned int dataSize)
{
	if (pixelCBdataAndSizePairs.at(index).first != nullptr)
		delete pixelCBdataAndSizePairs.at(index).first;

	pixelCBdataAndSizePairs.at(index).first = new char[dataSize];
	std::memcpy(pixelCBdataAndSizePairs.at(index).first, data, dataSize);
	pixelCBdataAndSizePairs.at(index).second = dataSize;
}

bool Material::doesCastShadow() const
{
	return castShadow;
}

bool Material::doesRecieveShadow() const
{
	return recieveShadow;
}

int Material::getRenderQueue() const
{
	return renderQueue;
}

void Material::getShadowAndRenderQueueStates(int& queue, bool& castShadow, bool& recieveShadow)
{
	queue = this->renderQueue;
	castShadow = this->castShadow;
	recieveShadow = this->recieveShadow;
}

void Material::bind(ID3D11DeviceContext* context) const
{
	if (prevBoundMaterial == const_cast<Material*>(this))
		return;
#if _DEBUG
	if (!isCompletlyInitialized[0] || !isCompletlyInitialized[1])
	{
		ErrorLogger::log("Material was not completly initialized before binding");
		exit(-1);
	}
#endif
	context->IASetPrimitiveTopology(topology);
	if (prevRasterizerState != rasterizerState)
		context->RSSetState(rasterizerState);
	if (prevdepthStencilState != depthStencilState)
		context->OMSetDepthStencilState(depthStencilState, 0);
	if (prevBlendState != blendState)
		context->OMSetBlendState(blendState, nullptr, 0xFFFFFF);
	if (prevSamplerState != samplerState1)
		context->PSSetSamplers(0, 1, &samplerState1);

	if (samplerState2 != nullptr)
		context->PSSetSamplers(1, 1, &samplerState2);
	if (samplerState3 != nullptr)
		context->PSSetSamplers(2, 1, &samplerState3);

	context->IASetInputLayout(vertexShader->getInputLayout());
	context->VSSetShader(vertexShader->getShader(), NULL, 0);

	if (prevPixelShader != pixelShader)
		context->PSSetShader(pixelShader->getShader(), NULL, 0);

	for (unsigned int index = 0; index < vertexConstantBuffers.size(); index++)
	{
		if (vertexCBdataAndSizePairs.at(index).first == nullptr)
			vertexConstantBuffers.at(index)->applyChanges();
		else
			vertexConstantBuffers.at(index)->applyChanges(vertexCBdataAndSizePairs.at(index).first, vertexCBdataAndSizePairs.at(index).second);
		context->VSSetConstantBuffers(index, 1, vertexConstantBuffers[index]->getAddressOf());
	}

	for (unsigned int index = 0; index < pixelConstantBuffers.size(); index++)
	{
		if (pixelCBdataAndSizePairs.at(index).first == nullptr)
			pixelConstantBuffers.at(index)->applyChanges();
		else
			pixelConstantBuffers.at(index)->applyChanges(pixelCBdataAndSizePairs.at(index).first, pixelCBdataAndSizePairs.at(index).second);
		context->PSSetConstantBuffers(index, 1, pixelConstantBuffers[index]->getAddressOf());
	}

	prevBoundMaterial = const_cast<Material*>(this);

	prevdepthStencilState = depthStencilState;
	prevRasterizerState = rasterizerState;
	prevSamplerState = samplerState1;
	prevBlendState = blendState;
	prevVertexShader = vertexShader;
	prevPixelShader = pixelShader;
}

void Material::bind(ID3D11DeviceContext* context, PixelShader* overridePixelShader) const
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
	context->PSSetSamplers(0, 1, &samplerState1);
	context->VSSetShader(vertexShader->getShader(), NULL, 0);
	context->PSSetShader(overridePixelShader->getShader(), NULL, 0);

	for (unsigned int index = 0; index < vertexConstantBuffers.size(); index++)
		context->VSSetConstantBuffers(index, 1, vertexConstantBuffers[index]->getAddressOf());

	for (unsigned int index = 0; index < pixelConstantBuffers.size(); index++)
		context->PSSetConstantBuffers(index, 1, pixelConstantBuffers[index]->getAddressOf());
}

void Material::bind(ID3D11DeviceContext* context, VertexShader* overrideOvertexShader) const
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
	context->PSSetSamplers(0, 1, &samplerState1);
	context->VSSetShader(overrideOvertexShader->getShader(), NULL, 0);
	context->PSSetShader(pixelShader->getShader(), NULL, 0);

	for (unsigned int index = 0; index < vertexConstantBuffers.size(); index++)
		context->VSSetConstantBuffers(index, 1, vertexConstantBuffers[index]->getAddressOf());

	for (unsigned int index = 0; index < pixelConstantBuffers.size(); index++)
		context->PSSetConstantBuffers(index, 1, pixelConstantBuffers[index]->getAddressOf());
}

void Material::bind(ID3D11DeviceContext* context, VertexShader* overrideOvertexShader, PixelShader* overridePixelShader) const
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
	context->PSSetSamplers(0, 1, &samplerState1);
	context->VSSetShader(overrideOvertexShader->getShader(), NULL, 0);
	context->PSSetShader(overridePixelShader->getShader(), NULL, 0);

	for (unsigned int index = 0; index < vertexConstantBuffers.size(); index++)
		context->VSSetConstantBuffers(index, 1, vertexConstantBuffers[index]->getAddressOf());

	for (unsigned int index = 0; index < pixelConstantBuffers.size(); index++)
		context->PSSetConstantBuffers(index, 1, pixelConstantBuffers[index]->getAddressOf());
}

Material* const Material::getCurrentBoundMaterial()
{
	return prevBoundMaterial;
}