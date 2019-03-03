#pragma once
#include <d3d11.h>
#include <wrl\client.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")
#include "AdapterReader.h"
#include "Shader.h"

class Graphics
{
public:
	bool init(HWND hwnd, int width, int height);
	void renderFrame();
private:
	bool initDirectX(HWND hwnd, int width, int height);
	bool initShaders();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	VertexShader vertexShader;
};

