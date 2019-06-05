#pragma once
#include "Shader.h"
#include "ConstantBuffer.h"
#include <vector>
#include <d3d11.h>
#include <wrl\client.h>

enum class RenderQueue { OPAQUE_QUEUE, TRANSPARENT_QUEUE };

class Material
{
private:
	static Material* prevBoundMaterial;
	static ID3D11DepthStencilState* prevdepthStencilState;
	static ID3D11RasterizerState* prevRasterizerState;
	static ID3D11SamplerState* prevSamplerState;
	static ID3D11BlendState* prevBlendState;
	static VertexShader* prevVertexShader;
	static PixelShader* prevPixelShader;

	ID3D11DepthStencilState* depthStencilState = nullptr;
	ID3D11RasterizerState* rasterizerState = nullptr;
	ID3D11SamplerState* samplerState1 = nullptr;
	ID3D11SamplerState* samplerState2 = nullptr;
	ID3D11SamplerState* samplerState3 = nullptr;
	ID3D11BlendState* blendState = nullptr;

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;

	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<BaseVertexConstantBuffer*> vertexConstantBuffers;
	std::vector<BasePixelConstantBuffer*> pixelConstantBuffers;

	RenderQueue renderQueue = RenderQueue::OPAQUE_QUEUE;
	bool castShadow = true;
	bool recieveShadow = true;
#if _DEBUG
	bool isCompletlyInitialized[4] = { false, false, true, true };
#endif

public:
	Material();
	~Material();
	void setRenderStates(ID3D11DepthStencilState* depthStencilState, ID3D11RasterizerState* rasterizerState, ID3D11SamplerState* samplerState, ID3D11BlendState* blendState);
	void setSamplerState2(ID3D11SamplerState* samplerState);
	void setSamplerState3(ID3D11SamplerState* samplerState);
	void setShadowAndRenderQueueStates(RenderQueue queue, bool castShadow, bool recieveShadow);
	void setRenderQueue(RenderQueue queue);
	void setIfCastsShadow(bool castShadow);
	void setIfRecieveShadow(bool recieveShadow);
	void setShaders(VertexShader* vertexShader, PixelShader* pixelShader);
	void setTopologyType(D3D11_PRIMITIVE_TOPOLOGY topology);
	void addVertexConstantBuffer(BaseVertexConstantBuffer* constantBuffer);
	void addPixelConstantBuffer(BasePixelConstantBuffer* constantBuffer);
	bool doesCastShadow()const;
	bool doesRecieveShadow()const;
	RenderQueue getRenderQueue()const;
	void getShadowAndRenderQueueStates(RenderQueue& queue, bool& castShadow, bool& recieveShadow);
	void bind(ID3D11DeviceContext* context)const;
	void bind(ID3D11DeviceContext* context, PixelShader* overridePixelShader)const;
	void bind(ID3D11DeviceContext* context, VertexShader* overrideOvertexShader)const;
	void bind(ID3D11DeviceContext* context, VertexShader* overrideOvertexShader, PixelShader* overridePixelShader)const;
	static Material* const getCurrentBoundMaterial();
};