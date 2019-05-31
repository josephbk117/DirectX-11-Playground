#pragma once
#include <d3d11.h>
#include "..\\ErrorLogger.h"
class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	bool init(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencilView, int textureWidth, int textureHeight);
	void setRenderTarget();
	void clearRenderTarget(float red, float green, float blue, float alpha);
	ID3D11ShaderResourceView* getShaderResourceView();

private:
	ID3D11DeviceContext* m_context = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_renderTargetTexture = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
};

