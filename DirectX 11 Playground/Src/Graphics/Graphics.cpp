#include "Graphics.h"


bool Graphics::init(HWND hwnd, int width, int height)
{
	if (!initDirectX(hwnd, width, height))
		return false;

	if (!initShaders())
		return false;

	if (!initScene())
		return false;

	return true;
}

void Graphics::renderFrame()
{
	float bgColour[] = { 0.1f,0.1f,0.1f,1 };

	context->ClearRenderTargetView(renderTargetView.Get(), bgColour);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->IASetInputLayout(vertexShader.getInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	context->VSSetShader(vertexShader.getShader(), NULL, 0);
	context->PSSetShader(pixelShader.getShader(), NULL, 0);

	//Update constant buffers
	constantBuffer.data.xOffset = 0.5f;
	constantBuffer.data.yOffset = 0.5f;
	if (!constantBuffer.applyChanges())
		return;

	context->VSSetConstantBuffers(0, 1, constantBuffer.getAddressOf());

	UINT offset = 0;
	context->PSSetShaderResources(0, 1, texture.GetAddressOf());
	context->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
	context->IASetIndexBuffer(indicesBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
	//context->Draw(6, 0);
	context->DrawIndexed(indicesBuffer.getBufferSize(), 0, 0);

	swapchain->Present(1, NULL);
}

bool Graphics::initDirectX(HWND hwnd, int width, int height)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::log("No DXGI Adapters found");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;

	scd.Windowed = true;

	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION,
		&scd, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, context.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create device and swap chain");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backbuffer.GetAddressOf()));

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Get buffer failed");
		return false;
	}

	hr = device->CreateRenderTargetView(backbuffer.Get(), NULL, renderTargetView.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create render target view");
		return false;
	}

	//Depth/Stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Error creating depth-stencil buffer");
		return false;
	}

	hr = device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Error creating depth-stencil view");
		return false;
	}
	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Set depth-stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create depth stencil state.");
		return false;
	}

	//Create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//Set the vieport
	this->context->RSSetViewports(1, &viewport);

	//Create rasterizer desc
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	hr = device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Error creating rasterizer state");
		return false;
	}

	//Create sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Error creating sampler state");
		return false;
	}

	return true;
}

bool Graphics::initShaders()
{

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Debug\\";
#else  //x86 (Win32)
		shaderfolder = L"..\\Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Release\\";
#else  //x86 (Win32)
		shaderfolder = L"..\\Release\\";
#endif
#endif
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexShader.init(device, shaderfolder + L"vertexShader.cso", layout, numElements))
		return false;

	if (!pixelShader.init(device, shaderfolder + L"pixelShader.cso"))
		return false;

	return true;
}

bool Graphics::initScene()
{

	/*Vertex v[] = { Vertex(-0.5f,-0.5f, 1.0f, 0, 1), Vertex(-0.5f,0.5f,1.0f, 0.0f, 0.0f), Vertex(0.5f,-0.5f,1.0f,1,1),
					Vertex(0.5f,-0.5f,1.0f,1,1) ,Vertex(-0.5f,0.5f,1.0f,0.0f, 0.0f), Vertex(0.5f,0.5f,1.0f,1.0f, 0.0f) };*/

	Vertex v[] = { Vertex(-0.5f,-0.5f, 1.0f, 0, 1), Vertex(-0.5f,0.5f,1.0f, 0.0f, 0.0f), Vertex(0.5f,0.5f,1.0f,1.0f, 0.0f), Vertex(0.5f, -0.5f,1.0f,1.0f, 1.0f) };

	DWORD indices[] =
	{
		0,1,2,
		0,2,3
	};

	HRESULT hr = vertexBuffer.init(device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create vertex buffer");
		return false;
	}

	hr = indicesBuffer.init(device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create index buffer");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Resources\\Textures\\bitshiftProductions.png", nullptr, texture.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create WIC texture from file");
		return false;
	}

	hr = constantBuffer.init(device.Get(), context.Get());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create constant buffer");
		return false;
	}

	return true;
}