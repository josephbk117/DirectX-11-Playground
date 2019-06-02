#include "Graphics.h"
#include "RenderTexture.h"
#include "Primitive3DModels.h"
#include "Animator.h"
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
		float dist1 = XMVector3Length((refCameraPos - lhs->transform.GetPositionVector())).m128_f32[0];
		float dist2 = XMVector3Length((refCameraPos - rhs->transform.GetPositionVector())).m128_f32[0];
		return dist1 > dist2;
	}
};

struct TransparentRenderableCompare
{
	bool operator()(Renderable* lhs, Renderable* rhs)
	{
		float dist1 = XMVector3Length((refCameraPos - lhs->transform.GetPositionVector())).m128_f32[0];
		float dist2 = XMVector3Length((refCameraPos - rhs->transform.GetPositionVector())).m128_f32[0];
		return dist1 < dist2;
	}
};

void Graphics::renderFrame()
{
	static float t_time = 0;
	static bool drawDebug = true;
	static float ambientLightIntensity = 0.1f;
	static DirectX::XMVECTOR lightDir = { 0.8f, 0, 0 };
	t_time += 0.01f;

	dirLight.setRotation(lightDir);

	//Update all constant buffers
	//skinnedModel.animate(t_time, &vertexSkinnedInfoConstantBuffer.data.jointMatrices[0]);

	for (int i = 0; i < renderables.size(); i++)
	{
		if (renderables.at(i).getIfSkinnedModel())
			Animator::animate(t_time, &vertexSkinnedInfoConstantBuffer.data.jointMatrices[0], &renderables[i]);
	}

	vertexSkinnedInfoConstantBuffer.data.mvpMatrix = vertexInfoConstantBuffer.data.mvpMatrix;
	vertexSkinnedInfoConstantBuffer.data.jointMatrices[0] = vertexSkinnedInfoConstantBuffer.data.jointMatrices[0] * vertexSkinnedInfoConstantBuffer.data.jointMatrices[0];
	vertexSkinnedInfoConstantBuffer.data.jointMatrices[1] = vertexSkinnedInfoConstantBuffer.data.jointMatrices[1] * vertexSkinnedInfoConstantBuffer.data.jointMatrices[1];
	vertexSkinnedInfoConstantBuffer.data.jointMatrices[2] = vertexSkinnedInfoConstantBuffer.data.jointMatrices[2] * vertexSkinnedInfoConstantBuffer.data.jointMatrices[2];
	if (!vertexSkinnedInfoConstantBuffer.applyChanges())
		return;

	vertexInfoConstantBuffer.data.mvpMatrix = DirectX::XMMatrixIdentity() * camera.GetMatrix() * camera.GetProjectionMatrix();
	vertexInfoConstantBuffer.data.mvpMatrix = DirectX::XMMatrixTranspose(vertexInfoConstantBuffer.data.mvpMatrix);
	if (!vertexInfoConstantBuffer.applyChanges())
		return;

	vertexInfoLightingBuffer.data.lightMatrix = dirLight.GetLightMatrix();
	if (!vertexInfoLightingBuffer.applyChanges())
		return;

	pixelInfoLightingBuffer.data.ambientLightIntensity = ambientLightIntensity;
	pixelInfoLightingBuffer.data.ambientLightColour = DirectX::XMFLOAT3(1, 1, 0);
	if (!pixelInfoLightingBuffer.applyChanges())
		return;

	if (!pixelUnlitBasicBuffer.applyChanges())
		return;

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

	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	context->ClearRenderTargetView(renderTargetView.Get(), bgColour);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Add all opaque objects to set, and comparision function is distance from camera
	refCameraPos = camera.GetPositionVector();
	std::set<Renderable*, OpaqueRenderableCompare> opaqueRenderables;
	std::set<Renderable*, TransparentRenderableCompare> transparentRenderables;
	for (unsigned int i = 0; i < renderables.size(); i++)
	{
		if (renderables.at(i).getMaterial()->getRenderQueue() == RenderQueue::OPAQUE_QUEUE)
			opaqueRenderables.insert(&renderables[i]);
		else if (renderables.at(i).getMaterial()->getRenderQueue() == RenderQueue::TRANSPARENT_QUEUE)
			transparentRenderables.insert(&renderables[i]);
	}

	//Render all opaque objects

	for (std::set<Renderable*, OpaqueRenderableCompare>::iterator it = opaqueRenderables.begin(); it != opaqueRenderables.end(); ++it)
	{
		(*it)->setShadowMapTexture(dirLight.getShadowMapRenderTexture());
		(*it)->draw(context.Get(), camera.GetMatrix() * camera.GetProjectionMatrix());
	}

	//Draw skybox
	skyboxMaterial.bind(context.Get());
	skybox.draw(camera.GetViewDirectionMatrix() * camera.GetProjectionMatrix());
	//Render all transparent objects

	for (std::set<Renderable*, TransparentRenderableCompare>::iterator it = transparentRenderables.begin(); it != transparentRenderables.end(); ++it)
	{
		(*it)->setShadowMapTexture(dirLight.getShadowMapRenderTexture());
		(*it)->draw(context.Get(), camera.GetMatrix() * camera.GetProjectionMatrix());
	}

	//context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	////Set up skinned mesh shader
	//regularSkinnedMaterial.bind(context.Get());
	//skinnedModel.draw(DirectX::XMMatrixTranslation(0, 1, 4), camera.transform.GetMatrix() * camera.GetProjectionMatrix());
	//skinnedModel.draw(DirectX::XMMatrixTranslation(1, 1, 4), camera.transform.GetMatrix() * camera.GetProjectionMatrix());
	//skinnedModel.draw(DirectX::XMMatrixTranslation(-1, 1, 4), camera.transform.GetMatrix() * camera.GetProjectionMatrix());

	////Set up regular shader
	//regularMaterial.bind(context.Get());

	//models[0].setTexture(texture.Get());
	//models[0].setTexture2(renderTexture.getShaderResourceView());
	//models[0].draw(DirectX::XMMatrixIdentity(), camera.transform.GetMatrix() * camera.GetProjectionMatrix());

	//unlitScreenRenderingMaterial.bind(context.Get());

	//models[1].setTexture(renderTexture.getShaderResourceView());
	//models[1].draw(DirectX::XMMatrixScaling(2, 2, 2)* DirectX::XMMatrixTranslation(0, 4, 4), camera.transform.GetMatrix() * camera.GetProjectionMatrix());

	//skybox.draw(camera.GetViewDirectionMatrix() * camera.GetProjectionMatrix());

	////Debug stage, Draw OBBs and other gizmo visualization items
	//FXMVECTOR forward = camera.transform.GetForwardVector();

	//static Ray ray(camera.transform.GetPositionFloat3(), { forward.m128_f32[0] , 0 , forward.m128_f32[2] });
	//ray.setOrigin(DirectX::XMVECTOR{ 0, 2, -2, 0 });
	//static float val[3] = { 0,1,0 };
	//for (unsigned int meshIndex = 0; meshIndex < models[0].getMeshes().size(); meshIndex++)
	//{
	//	drawDebug = models[0].getMeshes().at(meshIndex).getOBB().doesRayIntersect(ray);
	//	if (drawDebug)
	//		break;
	//}
	//debugViewRenderingMaterial.bind(context.Get());

	//if (drawDebug)
	//{
	//	models[0].drawDebugView(DirectX::XMMatrixIdentity(), camera.transform.GetMatrix() * camera.GetProjectionMatrix());
	//}
	//ray.draw(device.Get(), context.Get(), vertexInfoConstantBuffer, camera.transform.GetMatrix() * camera.GetProjectionMatrix());
	////Start rendering on to depth render texture
	//renderTexture.setRenderTarget();
	//renderTexture.clearRenderTarget(1, 1, 1, 1);

	//depthRenderingMaterial.bind(context.Get());

	//models[0].draw(DirectX::XMMatrixIdentity(), dirLight.GetLightMatrix());

	//context->IASetInputLayout(skinnedVertexShader.getInputLayout());
	//context->VSSetShader(skinnedVertexShader.getShader(), NULL, 0);

	//skinnedModel.draw(DirectX::XMMatrixTranslation(0, 1, 4), dirLight.GetLightMatrix());
	//skinnedModel.draw(DirectX::XMMatrixTranslation(1, 1, 4), dirLight.GetLightMatrix());
	//skinnedModel.draw(DirectX::XMMatrixTranslation(-1, 1, 4), dirLight.GetLightMatrix());

	////Start rendering on top default render texture
	//context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

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
	ImGui::SliderFloat("Animation timeline", &t_time, 0.0f, 100.0f, "%.2f");
	ImGui::SliderFloat3("Light dir", &lightDir.m128_f32[0], -1.0f, 1.0f, "%.2f");
	/*if (ImGui::SliderFloat3("Look dir", &val[0], -1.0f, 1.0f, "%.2f"))
	{
		ray.setDirection(DirectX::XMVECTOR{ val[0], val[1], val[2] });
	}*/
	ImGui::ColorEdit3("Bounding box colour", &pixelUnlitBasicBuffer.data.colour.m128_f32[0]);
	ImGui::Checkbox("Enable bounding box display", &drawDebug);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	swapchain->Present(0, NULL);
}

void Graphics::onWindowResized(HWND hwnd, int width, int height)
{
	//if (swapchain.Get() != nullptr)
	//{
	//	context->OMSetRenderTargets(0, 0, 0);
	//	context->ClearState();
	//	renderTargetView->Release();

	//	HRESULT hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	//	/*if (FAILED(hr))
	//		ErrorLogger::log(hr, "Swapchain buffer resize failed");*/
	//	//error hadling
	//	// Get buffer and create a render-target-view.
	//	depthStencilBuffer.Reset();
	//	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&depthStencilBuffer);
	//	if (FAILED(hr))
	//		ErrorLogger::log(hr, "Swapchain buffer creation failed");
	//	// Perform error handling here!

	//	hr = device->CreateRenderTargetView(depthStencilBuffer.Get(), NULL, renderTargetView.GetAddressOf());
	//	if (FAILED(hr))
	//		ErrorLogger::log(hr, "Render target view creation failed");
	//	// Perform error handling here!

	//	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);

	//	// Set up the viewport.
	//	D3D11_VIEWPORT vp;
	//	vp.Width = width;
	//	vp.Height = height;
	//	vp.MinDepth = 0.0f;
	//	vp.MaxDepth = 1.0f;
	//	vp.TopLeftX = 0;
	//	vp.TopLeftY = 0;
	//	context->RSSetViewports(1, &vp);
	//}
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
		viewport.Width = width;
		viewport.Height = height;
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

		hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
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
		renderTexture.init(device.Get(), context.Get(), depthStencilView.Get(), width, height);
	}
	catch (COMException & e)
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

	return true;
}

bool Graphics::initScene()
{
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Resources\\Textures\\cottage_diffuse.png", nullptr, texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from file");

		hr = vertexInfoConstantBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = vertexSkinnedInfoConstantBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = vertexInfoLightingBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = pixelInfoLightingBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		hr = pixelUnlitBasicBuffer.init(device.Get(), context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create constant buffer");

		regularMaterial.setRenderStates(defaultDepthStencilState.Get(), defaultRasterizerState.Get(), samplerState.Get(), disabledBlendState.Get());
		regularMaterial.setShaders(&vertexShader, &pixelShader);
		regularMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		regularMaterial.addVertexConstantBuffer(&vertexInfoLightingBuffer);
		regularMaterial.addPixelConstantBuffer(&pixelInfoLightingBuffer);

		regularSkinnedMaterial.setRenderStates(defaultDepthStencilState.Get(), defaultRasterizerState.Get(), samplerState.Get(), disabledBlendState.Get());
		regularSkinnedMaterial.setShaders(&skinnedVertexShader, &pixelShader);
		regularSkinnedMaterial.addVertexConstantBuffer(&vertexSkinnedInfoConstantBuffer);
		regularSkinnedMaterial.addPixelConstantBuffer(&pixelInfoLightingBuffer);

		depthRenderingMaterial.setRenderStates(defaultDepthStencilState.Get(), lightDepthRenderingRasterizerState.Get(), samplerState.Get(), disabledBlendState.Get());
		depthRenderingMaterial.setShaders(&vertexShader, &depthBasicShader);
		depthRenderingMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);

		unlitScreenRenderingMaterial.setRenderStates(depthTestDisabledDepthStencilState.Get(), debugRasterizerState.Get(), samplerState.Get(), defaultBlendState.Get());
		unlitScreenRenderingMaterial.setShaders(&vertexShader, &unlitTransparentPixelShader);
		unlitScreenRenderingMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		unlitScreenRenderingMaterial.setRenderQueue(RenderQueue::TRANSPARENT_QUEUE);

		skyboxMaterial.setRenderStates(defaultDepthStencilState.Get(), debugRasterizerState.Get(), samplerState.Get(), disabledBlendState.Get());
		skyboxMaterial.setShaders(&vertexShader, &unlitBasicPixelShader);
		skyboxMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);

		debugViewRenderingMaterial.setRenderStates(defaultDepthStencilState.Get(), debugRasterizerState.Get(), samplerState.Get(), disabledBlendState.Get());
		debugViewRenderingMaterial.setShaders(&vertexShader, &unlitBasicPixelShader);
		debugViewRenderingMaterial.addVertexConstantBuffer(&vertexInfoConstantBuffer);
		debugViewRenderingMaterial.addPixelConstantBuffer(&pixelUnlitBasicBuffer);
		debugViewRenderingMaterial.setTopologyType(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);


		Model* model;
		model = new Model;
		if (!model->init("Resources\\Models\\cottage_obj.obj", device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&regularMaterial, model);

		model = new Model;
		if (!model->init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&unlitScreenRenderingMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(2, 2, 5);

		model = new Model;
		if (!model->init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&unlitScreenRenderingMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(1.5f, 2, 4);
		renderables.at(renderables.size() - 1).transform.SetRotation(0, 0, 45);

		model = new Model;
		if (!model->init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&unlitScreenRenderingMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(2, 2, 3);

		model = new Model;
		if (!model->init("Resources\\Models\\stairsLong.obj", device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		renderables.emplace_back(&regularMaterial, model);
		renderables.at(renderables.size() - 1).transform.SetPosition(0, 1, -3);

		SkinnedModel* skinnedModel = new SkinnedModel;
		if (!skinnedModel->init("Resources\\Models\\animCylinder.fbx", device.Get(), context.Get(), texture.Get(), vertexSkinnedInfoConstantBuffer))
			return false;
		renderables.emplace_back(&regularSkinnedMaterial, skinnedModel);
		renderables.at(renderables.size() - 1).transform.SetPosition(0, 2, 0);

		dirLight.enableShadowMapRendering(&renderTexture);

		/*Model model1;
		if (!model1.init("Resources\\Models\\cottage_obj.obj", device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		models.push_back(model1);

		Model model2;
		if (!model2.init(Primitive3DModels::QUAD.vertices, Primitive3DModels::QUAD.indices, device.Get(), context.Get(), texture.Get(), vertexInfoConstantBuffer))
			return false;
		models.push_back(model2);

		if (!skinnedModel.init("Resources\\Models\\animCylinder.fbx", device.Get(), context.Get(), texture.Get(), vertexSkinnedInfoConstantBuffer))
			return false;*/

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
	catch (COMException & e)
	{
		ErrorLogger::log(e);
		return false;
	}
	return true;
}