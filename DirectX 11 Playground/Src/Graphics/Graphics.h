#pragma once
#include <d3d11.h>
#include "TextureManager.h"
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
#include "Particle System/ParticleSystem.h"
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
	VertexShader postProcessingVertexShader;
	PixelShader pixelShader;
	PixelShader unlitBasicPixelShader;
	PixelShader unlitTransparentPixelShader;
	PixelShader depthBasicShader;
	PixelShader postProcessingPixelShader;

	VertexConstantBuffer<CB_VS_VertexShader> vertexInfoConstantBuffer;
	VertexConstantBuffer<CB_VS_Skinned_VertexShader> vertexSkinnedInfoConstantBuffer;
	VertexConstantBuffer<CB_VS_LightBuffer> vertexInfoLightingBuffer;
	PixelConstantBuffer<CB_PS_LightBuffer> pixelInfoLightingBuffer;
	PixelConstantBuffer<CB_PS_UnlitBasic> pixelUnlitBasicBuffer;

	RenderTexture lightDepthRenderTexture;
	RenderTexture postProcessingRenderTexture;
	CubemapTexture cubemap;
	Skybox skybox;
	std::vector<Renderable> renderables;
	Model postProcessingQuad;
	DirectionalLight dirLight;

	Material regularMaterial;
	Material regularSkinnedMaterial;
	Material depthRenderingMaterial;
	Material unlitScreenRenderingMaterial;
	Material unlitMaterial;
	Material skyboxMaterial;
	Material debugViewRenderingMaterial;
	Material postProcessingMaterial;

	ParticleSystem particleSystem;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthTestDisabledDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> defaultRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> debugRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> lightDepthRenderingRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSamplerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> defaultBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> disabledBlendState;

	Timer timer;

};