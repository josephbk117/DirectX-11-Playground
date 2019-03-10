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

	context->IASetInputLayout(vertexShader.getInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(vertexShader.getShader(), NULL, 0);
	context->PSSetShader(pixelShader.getShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	context->Draw(3, 0);

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
	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);

	//Create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	viewport.Width = width;
	viewport.Height = height;
	//Set the vieport
	this->context->RSSetViewports(1, &viewport);

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
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
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

	Vertex v[] = { Vertex(-0.5f,-0.5f, 1,0,0), Vertex(0.0f,0.5f,0,1,0), Vertex(0.5f,-0.5f,0,0,1) };

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

	vertexBufferData.pSysMem = v;

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create vertex buffer");
		return false;
	}

	return true;
}