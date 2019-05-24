#pragma once
#include <d3d11.h>
#include <wrl\client.h>
#include <WICTextureLoader.h>
#include "..\ErrorLogger.h"

//TODO : Support different types of cubemaps

class CubemapTexture
{
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textures[6];
public:
	// Order of image file paths : Front, Back, Left, Right, Bottom, Top
	bool init(ID3D11Device* device, const std::wstring imagePaths[6])
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, imagePaths[i].c_str(), nullptr, textures[i].GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from file for Cubemap");
		}
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>* getTextures()
	{
		return textures;
	}

};