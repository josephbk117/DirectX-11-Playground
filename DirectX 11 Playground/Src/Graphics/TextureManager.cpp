#include "TextureManager.h"
#include "..\\ErrorLogger.h"
TextureManager* TextureManager::instance = nullptr;
void TextureManager::init(ID3D11Device* device)
{
#if _DEBUG
	if (instance != nullptr)
		ErrorLogger::log("Texture Manager has already been initialized, init() called multiple times");
#endif
	instance = new TextureManager;
	instance->m_device = device;
}

void TextureManager::shutdown()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::addTexture(const std::wstring& filePath, const std::string& textureName)
{
#if _DEBUG
	if (instance == nullptr)
	{
		ErrorLogger::log("Texture manager was not initialized befor invoking addTexture()");
		return;
	}
#endif
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;
	HRESULT hr = DirectX::CreateWICTextureFromFile(instance->m_device, filePath.c_str(), nullptr, resourceView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from file");

	instance->m_textureNameMap.insert(std::pair<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(textureName, resourceView));
}

ID3D11ShaderResourceView* TextureManager::getTexture(const std::string& textureName)
{
#if _DEBUG
	if (instance == nullptr)
	{
		ErrorLogger::log("Texture manager was not initialized befor invoking getTexture()");
		return nullptr;
	}
	auto it = instance->m_textureNameMap.find(textureName);
	if (it == instance->m_textureNameMap.end())
	{
		ErrorLogger::log("Texture manager does not contain texture by name of " + textureName);
		return nullptr;
	}
#endif
	return instance->m_textureNameMap.at(textureName).Get();
}
