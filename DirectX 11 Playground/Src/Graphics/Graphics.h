#pragma once
#include <d3d11.h>
#include <wrl\client.h>
#include <WICTextureLoader.h>
#include "AdapterReader.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "RenderTexture.h"
#include "..\\Timer.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

class Graphics
{
public:
	bool init(HWND hwnd, int width, int height);
	void renderFrame();
	Camera camera;

private:
	bool initDirectX(HWND hwnd, int width, int height);
	bool initShaders();
	bool initScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	VertexShader vertexShader;
	PixelShader pixelShader;
	ConstantBuffer<CB_VS_VertexShader> constantBuffer;
	RenderTexture renderTexture;
	Model model;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	Timer timer;
};

