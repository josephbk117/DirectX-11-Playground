#pragma once
#include <d3d11.h>
#include <wrl\client.h>
#include <WICTextureLoader.h>
#include "AdapterReader.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "SkinnedModel.h"
#include "RenderTexture.h"
#include "Light.h"
#include "CubemapTexture.h"
#include "Skybox.h"
#include "Material.h"
#include "Renderable.h"
#include "..\\Timer.h"
#include "Renderable.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

class Graphics
{
public:
	bool init(HWND hwnd, int width, int height);
	void renderFrame();
	void onWindowResized(HWND hwnd, int width, int height);
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
	VertexShader skinnedVertexShader;
	PixelShader pixelShader;
	PixelShader unlitBasicPixelShader;
	PixelShader depthBasicShader;

	VertexConstantBuffer<CB_VS_VertexShader> vertexInfoConstantBuffer;
	VertexConstantBuffer<CB_VS_Skinned_VertexShader> vertexSkinnedInfoConstantBuffer;
	VertexConstantBuffer<CB_VS_LightBuffer> vertexInfoLightingBuffer;
	PixelConstantBuffer<CB_PS_LightBuffer> pixelInfoLightingBuffer;
	PixelConstantBuffer<CB_PS_UnlitBasic> pixelUnlitBasicBuffer;

	RenderTexture renderTexture;
	CubemapTexture cubemap;
	Skybox skybox;
	//std::vector<Model> models;
	std::vector<Renderable> renderables;
	//SkinnedModel skinnedModel;
	DirectionalLight dirLight;

	Material regularMaterial;
	Material regularSkinnedMaterial;
	Material depthRenderingMaterial;
	Material unlitScreenRenderingMaterial;
	Material debugViewRenderingMaterial;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> defaultRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> debugRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> lightDepthRenderingRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> defaultBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> disabledBlendState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	Timer timer;

};