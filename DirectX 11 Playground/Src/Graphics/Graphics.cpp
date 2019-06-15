#include "Graphics.h"
#include "RenderTexture.h"
#include "Primitive3DModels.h"
#include "Animator.h"
#include "DebugViewer.h"
#include <set>

bool Graphics::init(HWND hwnd, int width, int height)
{
	timer.start();

	if (!initDirectX(hwnd, width, height))
		return false;

	if (!initShaders())
		return false;

	if (!initScene())
		return false;

	//Set up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	ImGui::StyleColorsDark();

	return true;
}


XMVECTOR refCameraPos;
struct OpaqueRenderableCompare
{
	bool operator()(Renderable* lhs, Renderable* rhs)
	{
		int lhsRenderQueueIndex = lhs->getMaterial()->getRenderQueue();
		int rhsRenderQueueIndex = rhs->getMaterial()->getRenderQueue();

		if (lhsRenderQueueIndex == rhsRenderQueueIndex)
		{
			float dist1 = XMVector3Length((refCameraPos - lhs->transform.GetPositionVector())).m128_f32[0];
			float dist2 = XMVector3Length((refCameraPos - rhs->transform.GetPositionVector())).m128_f32[0];
			return dist1 > dist2;
		}
		else
			return (lhsRenderQueueIndex < rhsRenderQueueIndex);
	}
};

struct TransparentRenderableCompare
{
	bool operator()(Renderable* lhs, Renderable* rhs)
	{
		int lhsRenderQueueIndex = lhs->getMaterial()->getRenderQueue();
		int rhsRenderQueueIndex = rhs->getMaterial()->getRenderQueue();

		if (lhsRenderQueueIndex == rhsRenderQueueIndex)
		{
			float dist1 = XMVector3Length((refCameraPos - lhs->transform.GetPositionVector())).m128_f32[0];
			float dist2 = XMVector3Length((refCameraPos - rhs->transform.GetPositionVector())).m128_f32[0];
			return dist1 < dist2;
		}
		else
			return (rhsRenderQueueIndex < lhsRenderQueueIndex);
	}
};

void Graphics::renderFrame()
{
	static float t_time = 0;
	static float ambientLightIntensity = 0.1f;
	static DirectX::XMVECTOR ambientLightColour = { 0.3f, 0.3f, 0.3f };
	static DirectX::XMVECTOR lightDir = { 0.8f, 0, 0 };
	static float shadowBias = 0.001f;
	static bool renderWithPostProcessing = false;
	t_time += 0.01f;

	dirLight.setRotation(lightDir);
	for (Renderable& renderable : renderables)
	{
		if (renderable.getIfSkinnedModel())
			Animator::animate(t_time, &vertexSkinnedInfoConstantBuffer.data.jointMatrices[0], &renderable);
	}

	vertexSkinnedInfoConstantBuffer.data.mvpMatrix = vertexInfoConstantBuffer.data.mvpMatrix;
	vertexSkinnedInfoConstantBuffer.data.jointMatrices[0] = vertexSkinnedInfoConstantBuffer.data.jointMatrices[0] * vertexSkinnedInfoConstantBuffer.data.jointMatrices[0];
	vertexSkinnedInfoConstantBuffer.data.jointMatrices[1] = vertexSkinnedInfoConstantBuffer.data.jointMatrices[1] * vertexSkinnedInfoConstantBuffer.data.jointMatrices[1];
	vertexSkinnedInfoConstantBuffer.data.jointMatrices[2] = vertexSkinnedInfoConstantBuffer.data.jointMatrices[2] * vertexSkinnedInfoConstantBuffer.data.jointMatrices[2];

	vertexInfoConstantBuffer.data.mvpMatrix = DirectX::XMMatrixIdentity() * camera.GetMatrix() * camera.GetProjectionMatrix();
	//vertexInfoConstantBuffer.data.mvpMatrix = DirectX::XMMatrixTranspose(vertexInfoConstantBuffer.data.mvpMatrix);

	vertexInfoLightingBuffer.data.lightMatrix = dirLight.GetLightMatrix();
	vertexInfoLightingBuffer.data.lightDirection = dirLight.getDirection();

	pixelInfoLightingBuffer.data.ambientLightIntensity = ambientLightIntensity;
	XMStoreFloat3(&pixelInfoLightingBuffer.data.ambientLightColour, ambientLightColour);
	pixelInfoLightingBuffer.data.bias = shadowBias;

	//pixelUnlitBasicBuffer.data.colour = { 1,1,1,0 };

	float bgColour[] = { 0.1f,0.1f,0.1f,1 };

	/*
	Rendering stages:
	1. Directional light depth rendering for all objects that cast shadow [ Shadow map ]
	2. In default render texture, Render all opaque objects with shadow map applied
	3. Draw skybox
	4. Render all transparent objects
	5. Draw all debug visualizations
	*/

	if (dirLight.doesLightRenderShadowMap())
	{
		dirLight.bindShadowMapRenderTexture();
		//Render all shadow casting objects
		for (int i = 0; i < renderables.size(); i++)
		{
			if (renderables.at(i).getMaterial()->doesCastShadow())
				renderables.at(i).overridePixelShaderDraw(context.Get(), &depthBasicShader, dirLight.GetLightMatrix());
		}
	}

	//Add all opaque objects to set, and comparision function is distance from camera
	refCameraPos = camera.GetPositionVector();
	std::set<Renderable*, OpaqueRenderableCompare> opaqueRenderables;
	std::set<Renderable*, TransparentRenderableCompare> transparentRenderables;
	std::set<Renderable*, OpaqueRenderableCompare> postProcessedRenderables;
	for (unsigned int i = 0; i < renderables.size(); i++)
	{
		int renderQueueIndex = renderables.at(i).getMaterial()->getRenderQueue();
		if (renderQueueIndex >= RenderQueue::OPAQUE_QUEUE && renderQueueIndex < RenderQueue::TRANSPARENT_QUEUE)
			opaqueRenderables.insert(&renderables[i]);
		else if (renderQueueIndex >= RenderQueue::TRANSPARENT_QUEUE && renderQueueIndex < RenderQueue::POST_PROCESSING_QUEUE)
			transparentRenderables.insert(&renderables[i]);
		else if (renderQueueIndex >= RenderQueue::POST_PROCESSING_QUEUE)
			postProcessedRenderables.insert(&renderables[i]);
	}

	if (!renderWithPostProcessing)
	{
		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
		context->ClearRenderTargetView(renderTargetView.Get(), bgColour);
		context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	else
	{
		postProcessingRenderTexture.setRenderTarget();
		postProcessingRenderTexture.clearRenderTarget(0, 0, 0, 1);
	}

	//Render all opaque objects

	for (std::set<Renderable*, OpaqueRenderableCompare>::iterator it = opaqueRenderables.begin(); it != opaqueRenderables.end(); ++it)
	{
		(*it)->setShadowMapTexture(dirLight.getShadowMapRenderTexture());
		(*it)->draw(context.Get(), camera.GetMatrix() * camera.GetProjectionMatrix());
	}

	particleSystem.update(1.0f / 600.0f);
	particleSystem.draw(context.Get(), camera, vertexInfoConstantBuffer);

	//Draw skybox

	skyboxMaterial.bind(context.Get());
	skybox.draw(camera.GetViewDirectionMatrix() * camera.GetProjectionMatrix());

	//Render all transparent objects

	for (std::set<Renderable*, TransparentRenderableCompare>::iterator it = transparentRenderables.begin(); it != transparentRenderables.end(); ++it)
	{
		(*it)->setShadowMapTexture(dirLight.getShadowMapRenderTexture());
		(*it)->draw(context.Get(), camera.GetMatrix() * camera.GetProjectionMatrix());
	}

	//Render post processed views
	for (std::set<Renderable*, OpaqueRenderableCompare>::iterator it = postProcessedRenderables.begin(); it != postProcessedRenderables.end(); ++it)
	{
		(*it)->draw(context.Get(), camera.GetMatrix() * camera.GetProjectionMatrix());
	}



	DebugViewer::setColour(1, 1, 0);
	DebugViewer::startDebugView(context.Get());

	static Ray ray1;
	ray1.setDirection(XMVECTOR{ 1, 1, 0 });
	ray1.setOrigin(XMFLOAT3{ 0, 2, 0 });
	ray1.draw(device.Get(), context.Get(), vertexInfoConstantBuffer, camera.GetMatrix() * camera.GetProjectionMatrix());
	DebugViewer::endDebugView(context.Get());

	DebugViewer::setColour(0, 1, 1);
	DebugViewer::startDebugView(context.Get());
	static Ray ray2;
	ray2.setDirection(XMVECTOR{ -1,1,0 });
	ray2.setOrigin(XMFLOAT3{ 0,2,0 });
	ray2.draw(device.Get(), context.Get(), vertexInfoConstantBuffer, camera.GetMatrix() * camera.GetProjectionMatrix());
	DebugViewer::endDebugView(context.Get());

	DebugViewer::setColour(0, 1, 0);
	DebugViewer::startDebugView(context.Get());
	static Ray ray3;
	ray3.setDirection(XMVECTOR{ 1,-1,0 });
	ray3.setOrigin(XMFLOAT3{ 0,2,0 });
	ray3.draw(device.Get(), context.Get(), vertexInfoConstantBuffer, camera.GetMatrix() * camera.GetProjectionMatrix());
	DebugViewer::endDebugView(context.Get());

	DebugViewer::setColour(1, 0, 0);
	DebugViewer::startDebugView(context.Get());
	static Ray ray4;
	ray4.setDirection(XMVECTOR{ -1, -1, 0 });
	ray4.setOrigin(XMFLOAT3{ 0, 2, 0 });
	ray4.draw(device.Get(), context.Get(), vertexInfoConstantBuffer, camera.GetMatrix() * camera.GetProjectionMatrix());

	static Ray ray5;
	ray5.setDirection(dirLight.getDirection());
	ray5.setOrigin(XMFLOAT3{ 0, 2, 0 });
	ray5.draw(device.Get(), context.Get(), vertexInfoConstantBuffer, camera.GetMatrix() * camera.GetProjectionMatrix());

	for (const Renderable& renderable : renderables)
	{
		ModelInterface* modelInterface = renderable.getModel();
		Model* model = dynamic_cast<Model*>(modelInterface);
		if (model == nullptr)
			continue;
		bool hasIntersected = false;
		for (const Mesh& mesh : model->getMeshes())
		{
			hasIntersected = mesh.getOBB().doesRayIntersect(ray5);
			if (hasIntersected)
				break;
		}

		if (hasIntersected)
			model->drawDebugView(renderable.transform.GetMatrix(), camera.GetMatrix() * camera.GetProjectionMatrix());
	}

	DebugViewer::endDebugView(context.Get());


	if (renderWithPostProcessing)
	{
		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
		context->ClearRenderTargetView(renderTargetView.Get(), bgColour);
		context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		postProcessingMaterial.bind(context.Get());
		postProcessingQuad.draw(XMMatrixIdentity(), XMMatrixIdentity());
	}

	//Start ImGui Frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Stuff");

	//Fps counter
	static int fpsCounter = 0;
	static std::string fpsString = "Frame rate";
	fpsCounter++;

	if (timer.getMillisecondsElapsed() > 1000.0)
	{
		fpsString = "Frame rate = " + std::to_string(fpsCounter);
		fpsCounter = 0;
		timer.restart();
	}

	ImGui::Text(fpsString.c_str());
	ImGui::SliderFloat("Ambient light intensity", &ambientLightIntensity, 0, 1, "%.2f");
	ImGui::ColorEdit3("Ambient light colour", &ambientLightColour.m128_f32[0]);
	ImGui::SliderFloat("Shadow bias", &shadowBias, 0.0f, 1.0f, "%.3f");
	ImGui::SliderFloat("Animation timeline", &t_time, 0.0f, 100.0f, "%.2f");
	ImGui::SliderFloat3("Light dir", &lightDir.m128_f32[0], -3.141f, 3.141f, "%.2f");
	ImGui::ColorEdit3("Bounding box colour", &pixelUnlitBasicBuffer.data.colour.m128_f32[0]);
	ImGui::Checkbox("Render with post processing", &renderWithPostProcessing);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	swapchain->Present(0, NULL);
}

void Graphics::onWindowResized(HWND hwnd, int width, int height)
{
	//swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	//DXGI_MODE_DESC modeDesc = { 0 };
	//modeDesc.Height = height;
	//modeDesc.Width = width;
	//modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//modeDesc.RefreshRate.Numerator = 60;
	//modeDesc.RefreshRate.Denominator = 1;
	//modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//swapchain->ResizeTarget(&modeDesc);
	//context->OMSetRenderTargets(0, 0, 0);
	//context->ClearState();
	//renderTargetView->Release();

	//depthStencilBuffer.Reset();
	//swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&depthStencilBuffer);

	//device->CreateRenderTargetView(depthStencilBuffer.Get(), NULL, renderTargetView.GetAddressOf());
	//context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);
	////if (swapchain.Get() != nullptr)
	////{
	////	context->OMSetRenderTargets(0, 0, 0);
	////	context->ClearState();
	////	renderTargetView->Release();

	////	HRESULT hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	////	/*if (FAILED(hr))
	////		ErrorLogger::log(hr, "Swapchain buffer resize failed");*/
	////	//error hadling
	////	// Get buffer and create a render-target-view.
	////	depthStencilBuffer.Reset();
	////	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&depthStencilBuffer);
	////	if (FAILED(hr))
	////		ErrorLogger::log(hr, "Swapchain buffer creation failed");
	////	// Perform error handling here!

	////	hr = device->CreateRenderTargetView(depthStencilBuffer.Get(), NULL, renderTargetView.GetAddressOf());
	////	if (FAILED(hr))
	////		ErrorLogger::log(hr, "Render target view creation failed");
	////	// Perform error handling here!

	////	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);

	//// Set up the viewport.
	//D3D11_VIEWPORT vp;
	//vp.Width = width;
	//vp.Height = height;
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	//vp.TopLeftX = 0;
	//vp.TopLeftY = 0;
	//context->RSSetViewports(1, &vp);

	camera.SetPerspectiveProjectionValues(60.0f, width / (float)height, 0.1f, 100.0f);
}

bool Graphics::initDirectX(HWND hwnd, int width, int height)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::log("No DXGI Adapters found");
			return false;
		}
		DXGI_SWAP_CHAIN_DESC scd = { 0 };

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

		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, NULL, nullptr, 0, D3D11_SDK_VERSION,
			&scd, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, context.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create device and swap chain");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backbuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Get buffer failed");

		hr = device->CreateRenderTargetView(backbuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view");

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
		COM_ERROR_IF_FAILED(hr, "Error creating depth-stencil buffer");

		hr = device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating depth-stencil view");

		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Set depth-stencil state
		D3D11_DEPTH_STENCIL_DESC depthstencildesc = { 0 };

		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->defaultDepthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state");

		//Set depth-stencil state with disabled depth testing
		depthstencildesc = { 0 };

		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthTestDisabledDepthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state");

		//Create viewport
		D3D11_VIEWPORT viewport = { 0 };

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		//Set the vieport
		this->context->RSSetViewports(1, &viewport);

		//Create default rasterizer desc
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = device->CreateRasterizerState(&rasterizerDesc, this->defaultRasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating default rasterizer state");

		//Create default rasterizer desc
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		hr = device->CreateRasterizerState(&rasterizerDesc, this->debugRasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating debugging rasterizer state");

		//Create default rasterizer desc
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = device->CreateRasterizerState(&rasterizerDesc, this->lightDepthRenderingRasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating light depth rendering rasterizer state");

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

		hr = device->CreateSamplerState(&samplerDesc, defaultSamplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating sampler state");

		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = device->CreateSamplerState(&samplerDesc, shadowSamplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating sampler state");

		//Create blend states
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
		ZeroMemory(&renderTargetBlendDesc, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));

		renderTargetBlendDesc.BlendEnable = true;
		renderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = renderTargetBlendDesc;

		hr = device->CreateBlendState(&blendDesc, defaultBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating Blend state");

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		ZeroMemory(&renderTargetBlendDesc, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
		renderTargetBlendDesc.BlendEnable = false;
		renderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = renderTargetBlendDesc;

		hr = device->CreateBlendState(&blendDesc, disabledBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Error creating Blend state");

		//Initialize an additional Rendertexture
		lightDepthRenderTexture.init(device.Get(), context.Get(), depthStencilView.Get(), width, height);
		postProcessingRenderTexture.init(device.Get(), context.Get(), depthStencilView.Get(), width, height);

		//Initialize Texture Manager
		TextureManager::init(device.Get());
	}
	catch (COMException& e)
	{
		ErrorLogger::log(e);
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
		{"NORMAL", 0 , DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);


	D3D11_INPUT_ELEMENT_DESC skinnedLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0 , DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"JOINT_IDS", 0 , DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"WEIGHTS", 0 , DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT skinnedNumElements = ARRAYSIZE(skinnedLayout);

	if (!vertexShader.init(device, shaderfolder + L"vertexShader.cso", layout, numElements))
		return false;

	if (!postProcessingVertexShader.init(device, shaderfolder + L"postProcessingVertexShader.cso", layout, numElements))
		return false;

	if (!skinnedVertexShader.init(device, shaderfolder + L"skinnedVertexShader.cso", skinnedLayout, skinnedNumElements))
		return false;

	if (!pixelShader.init(device, shaderfolder + L"pixelShader.cso"))
		return false;

	if (!unlitBasicPixelShader.init(device, shaderfolder + L"unlitBasic.cso"))
		return false;

	if (!unlitTransparentPixelShader.init(device, shaderfolder + L"unlitTransparent.cso"))
		return false;

	if (!depthBasicShader.init(device, shaderfolder + L"depthBasic.cso"))
		return false;

	if (!postProcessingPixelShader.init(device, shaderfolder + L"postProcessingPixelShader.cso"))
		return false;

	return true;
}

bool Graphics::initScene()
{
	try
	{
		TextureManager::addTexture(L"Resources\\Textures\\cottage_diffuse.png", "cottage_diffuse");
		TextureManager::addTexture(L"Resources\\Textures\\seamless_ground.jpg", "seamless_ground");
		TextureManager::addTexture(L"Resources\\Textures\\crate.jpg", "crate");

		HRESULT hr = vertexInfoConstantBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = vertexSkinnedInfoConstantBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = vertexInfoLightingBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = pixelInfoLightingBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = pixelUnlitBasicBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		regularMaterial.setRenderStates(defaultDepthStencilState.Get(), defaultRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		regularMaterial.setSamplerState2(shadowSamplerState.Get());
		regularMaterial.setShaders(&vertexShader, &pixelShader);
		regularMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		regularMaterial.addVertexConstantBuffer(&vertexInfoLightingBuffer);
		regularMaterial.addPixelConstantBuffer(&pixelInfoLightingBuffer);

		regularSkinnedMaterial.setRenderStates(defaultDepthStencilState.Get(), defaultRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		regularSkinnedMaterial.setShaders(&skinnedVertexShader, &pixelShader);
		regularSkinnedMaterial.addVertexConstantBuffer(&vertexSkinnedInfoConstantBuffer);
		regularSkinnedMaterial.addPixelConstantBuffer(&pixelInfoLightingBuffer);

		depthRenderingMaterial.setRenderStates(defaultDepthStencilState.Get(), lightDepthRenderingRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		depthRenderingMaterial.setShaders(&vertexShader, &depthBasicShader);
		depthRenderingMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);

		unlitScreenRenderingMaterial.setRenderStates(depthTestDisabledDepthStencilState.Get(), debugRasterizerState.Get(), defaultSamplerState.Get(), defaultBlendState.Get());
		unlitScreenRenderingMaterial.setShaders(&vertexShader, &unlitTransparentPixelShader);
		unlitScreenRenderingMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		unlitScreenRenderingMaterial.addPixelConstantBuffer(&pixelUnlitBasicBuffer);

		CB_PS_UnlitBasic tempUnlitBasic;
		tempUnlitBasic.colour = { 1,0,1,0.25f };

		unlitScreenRenderingMaterial.setPixelConstantBufferData(0, static_cast<void*>(&tempUnlitBasic), sizeof(CB_PS_UnlitBasic));
		unlitScreenRenderingMaterial.setRenderQueue(RenderQueue::TRANSPARENT_QUEUE);

		unlitMaterial.setRenderStates(defaultDepthStencilState.Get(), defaultRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		unlitMaterial.setShaders(&vertexShader, &unlitBasicPixelShader);
		unlitMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		unlitMaterial.addPixelConstantBuffer(&pixelUnlitBasicBuffer);

		skyboxMaterial.setRenderStates(defaultDepthStencilState.Get(), debugRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		skyboxMaterial.setShaders(&vertexShader, &unlitBasicPixelShader);
		skyboxMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		skyboxMaterial.addPixelConstantBuffer(&pixelUnlitBasicBuffer);

		tempUnlitBasic.colour = { 1,0,1,0 };
		skyboxMaterial.setPixelConstantBufferData(0, static_cast<void*>(&tempUnlitBasic), sizeof(CB_PS_UnlitBasic));

		debugViewRenderingMaterial.setRenderStates(defaultDepthStencilState.Get(), debugRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		debugViewRenderingMaterial.setShaders(&vertexShader, &unlitBasicPixelShader);
		debugViewRenderingMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		debugViewRenderingMaterial.addPixelConstantBuffer(&pixelUnlitBasicBuffer);
		debugViewRenderingMaterial.setTopologyType(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

		postProcessingMaterial.setRenderStates(defaultDepthStencilState.Get(), defaultRasterizerState.Get(), defaultSamplerState.Get(), disabledBlendState.Get());
		postProcessingMaterial.setShaders(&postProcessingVertexShader, &postProcessingPixelShader);
		postProcessingMaterial.setRenderQueue(RenderQueue::POST_PROCESSING_QUEUE);
		postProcessingMaterial.setIfCastsShadow(false);
		postProcessingMaterial.setIfRecieveShadow(false);

		DebugViewer::setDebugMaterialAndColourData(&debugViewRenderingMaterial, &pixelUnlitBasicBuffer.data.colour);
		dirLight.enableShadowMapRendering(&lightDepthRenderTexture);

		if (!postProcessingQuad.init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(), postProcessingRenderTexture.getShaderResourceView(), vertexInfoConstantBuffer))
			return false;

		Model* model;
		model = new Model;
		if (!model->init("Resources\\Models\\cottage_obj.fbx", device.Get(), context.Get(), TextureManager::getTexture("cottage_diffuse"), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&regularMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(0, -0.5f, 0);
		renderables.at(renderables.size() - 1).transform.SetRotation(DirectX::XM_PI / 2, 0, DirectX::XM_PI / 2);
		renderables.at(renderables.size() - 1).transform.SetScale(1, 0.5f, 1);

		model = new Model;
		if (!model->init("Resources\\Models\\box.fbx", device.Get(), context.Get(), TextureManager::getTexture("crate"), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&regularMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(-3, 1, 0);
		renderables.at(renderables.size() - 1).transform.SetScale(0.5f, 0.5f, 0.5f);

		Model quadModel;
		if (!quadModel.init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(), TextureManager::getTexture("seamless_ground"), vertexInfoConstantBuffer))
			return false;

		model = new Model;
		*model = quadModel;
		renderables.emplace_back(&regularMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(0.0f, -1, 0);
		renderables.at(renderables.size() - 1).transform.SetRotation(DirectX::XM_PI / 2, 0, DirectX::XM_PI / 2);
		renderables.at(renderables.size() - 1).transform.SetScale(10, 10, 10);

		model = new Model;
		*model = quadModel;
		renderables.emplace_back(&unlitScreenRenderingMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(2, 2, 5);

		model = new Model;
		*model = quadModel;
		renderables.emplace_back(&unlitScreenRenderingMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(1.5f, 2, 4);
		renderables.at(renderables.size() - 1).transform.SetRotation(0, 0, 45);

		model = new Model;
		*model = quadModel;
		renderables.emplace_back(&unlitScreenRenderingMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(2, 2, 3);

		model = new Model;
		if (!model->init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(),
			lightDepthRenderTexture.getShaderResourceView(), vertexInfoConstantBuffer,
			[](XMFLOAT3& vertex)
			{
				vertex.x *= 0.5f; vertex.y *= 0.5f; vertex.z *= 0.5;
				vertex.x -= 0.25f;
				vertex.y += 0.25f;
			}))
			return false;

			renderables.emplace_back(&postProcessingMaterial, model);

			SkinnedModel* skinnedModel = new SkinnedModel;
			if (!skinnedModel->init("Resources\\Models\\animCylinder.fbx", device.Get(), context.Get(),
				TextureManager::getTexture("crate"), vertexSkinnedInfoConstantBuffer, [](XMFLOAT3& vertex) { vertex.x *= 0.1f; vertex.y *= 0.1f; vertex.z *= 0.1f; }))
				return false;
			renderables.emplace_back(&regularSkinnedMaterial, skinnedModel);
			renderables.at(renderables.size() - 1).transform.SetPosition(0, 2, 0);

			ParticleSystemSettings particleSystemSettings;
			particleSystemSettings.emissionRate = 20;
			particleSystemSettings.gravity = 1;
			particleSystemSettings.maxLifetimeForParticle = 5;
			particleSystemSettings.maxParticles = 100;
			particleSystemSettings.material = &unlitMaterial;

			particleSystem.init(device.Get(), context.Get(), particleSystemSettings);

			std::wstring cubemapLocations[6];
			cubemapLocations[0] = L"Resources\\Textures\\Cubemaps\\Sahara Desert Cubemap\\sahara_ft.png";
			cubemapLocations[1] = L"Resources\\Textures\\Cubemaps\\Sahara Desert Cubemap\\sahara_bk.png";
			cubemapLocations[2] = L"Resources\\Textures\\Cubemaps\\Sahara Desert Cubemap\\sahara_lf.png";
			cubemapLocations[3] = L"Resources\\Textures\\Cubemaps\\Sahara Desert Cubemap\\sahara_rt.png";
			cubemapLocations[4] = L"Resources\\Textures\\Cubemaps\\Sahara Desert Cubemap\\sahara_dn.png";
			cubemapLocations[5] = L"Resources\\Textures\\Cubemaps\\Sahara Desert Cubemap\\sahara_up.png";
			cubemap.init(device.Get(), cubemapLocations);

			skybox.init(device.Get(), context.Get(), vertexInfoConstantBuffer, cubemap);

			camera.SetPosition(0.0f, 2.0f, -2.0f);
			camera.SetPerspectiveProjectionValues(60.0f, 1.0f, 0.1f, 100.0f);
	}
	catch (COMException& e)
	{
		ErrorLogger::log(e);
		return false;
	}
	return true;
}