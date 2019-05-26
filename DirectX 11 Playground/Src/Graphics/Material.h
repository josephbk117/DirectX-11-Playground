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

	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;

	std::vector<BaseConstantBuffer*> vertexConstantBuffers;
	std::vector<BaseConstantBuffer*> pixelConstantBuffers;

#if _DEBUG
	bool isCompletlyInitialized[4] = { false, false, true, true };
#endif

public:
	Material();
	~Material();
	void setRenderStates(ID3D11DepthStencilState* depthStencilState, ID3D11RasterizerState* rasterizerState, ID3D11SamplerState* samplerState);
	void setShaders(VertexShader* vertexShader, PixelShader* pixelShader);
	void setTopologyType(D3D11_PRIMITIVE_TOPOLOGY topology);
	void addVertexConstantBuffer(BaseConstantBuffer* constantBuffer);
	void addPixelConstantBuffer(BaseConstantBuffer* constantBuffer);
	void bind(ID3D11DeviceContext* context)const;
};