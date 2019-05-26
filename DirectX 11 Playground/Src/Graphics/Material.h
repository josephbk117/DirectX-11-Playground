#pragma once
#include "Shader.h"
#include "ConstantBuffer.h"
#include <vector>
#include <d3d11.h>
#include <wrl\client.h>

class Material
{
private:
	ID3D11DepthStencilState* depthStencilState = nullptr;
	ID3D11RasterizerState* rasterizerState = nullptr;
	ID3D11SamplerState* samplerState = nullptr;

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;

	std::vector<BaseConstantBuffer*> vertexConstantBuffers;
	std::vector<BaseConstantBuffer*> pixelConstantBuffers;

	bool isCompletlyInitialized[4] = { false, false, true, true };

public:
	Material();
	~Material();
	void setRenderStates(ID3D11DepthStencilState* depthStencilState, ID3D11RasterizerState* rasterizerState, ID3D11SamplerState* samplerState);
	void setShaders(VertexShader* vertexShader, PixelShader* pixelShader);
	void addVertexConstantBuffer(BaseConstantBuffer* constantBuffer);
	void addPixelConatantBuffer(BaseConstantBuffer* constantBuffer);
	void bind(ID3D11DeviceContext* context)const;
};