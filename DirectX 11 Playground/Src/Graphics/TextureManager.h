#pragma once
#include <map>
#include <string>
#include <wrl\client.h>
#include <WICTextureLoader.h>
#include <d3d11.h>
class TextureManager
{
private:
	static TextureManager* instance;
	ID3D11Device* m_device = nullptr;
	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textureNameMap;
public:
	static void init(ID3D11Device* device);
	static void shutdown();
	static void addTexture(const std::wstring& filePath, const std::string& textureName);
	static ID3D11ShaderResourceView* getTexture(const std::string& textureName);
};
