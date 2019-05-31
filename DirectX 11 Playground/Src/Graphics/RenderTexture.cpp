#include "RenderTexture.h"

RenderTexture::RenderTexture()
{
}


RenderTexture::RenderTexture(const RenderTexture& other)
{
}


RenderTexture::~RenderTexture()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = nullptr;
	}
}

bool RenderTexture::init(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencilView, int textureWidth, int textureHeight)
{
#if _DEBUG
	if (m_context != nullptr || m_depthStencilView != nullptr || m_renderTargetTexture != nullptr || m_renderTargetView != nullptr || m_shaderResourceView != nullptr)
		ErrorLogger::log("Render texture has already been initialized, Do not call init(...) more than once");
#endif

	this->m_context = context;
	this->m_depthStencilView = depthStencilView;

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	COM_ERROR_IF_FAILED(result, "Creating render target failed");

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	COM_ERROR_IF_FAILED(result, "Creating render target view failed");

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	COM_ERROR_IF_FAILED(result, "Creating shader resource view failed");

	return true;
}

void RenderTexture::setRenderTarget()
{
#if _DEBUG
	if (m_context == nullptr || m_depthStencilView == nullptr || m_renderTargetTexture == nullptr || m_renderTargetView == nullptr || m_shaderResourceView == nullptr)
		ErrorLogger::log("SetRenderTarget called before Render Texture has been initialized");
#endif
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return;
}

void RenderTexture::clearRenderTarget(float red, float green, float blue, float alpha)
{
#if _DEBUG
	if (m_context == nullptr || m_depthStencilView == nullptr || m_renderTargetTexture == nullptr || m_renderTargetView == nullptr || m_shaderResourceView == nullptr)
		ErrorLogger::log("ClearRenderTarget called before Render Texture has been initialized");
#endif
	float color[4];
	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_context->ClearRenderTargetView(m_renderTargetView, color);
	// Clear the depth buffer.
	m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return;
}

ID3D11ShaderResourceView* RenderTexture::getShaderResourceView()
{
	return m_shaderResourceView;
}