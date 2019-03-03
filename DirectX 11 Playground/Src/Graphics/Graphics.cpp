#include "Graphics.h"


bool Graphics::init(HWND hwnd, int width, int height)
{
	if (!initDirectX(hwnd, width, height))
		return false;

	if (!initShaders())
		return false;

	return true;
}

void Graphics::renderFrame()
{
	float bgColour[] = { 0,1,0,1 };

	context->ClearRenderTargetView(renderTargetView.Get(), bgColour);

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

	return true;
}

bool Graphics::initShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexShader.init(device, L"..\\x64\\Debug\\vertexShader.cso", layout, numElements))
		return false;


	return true;
}
