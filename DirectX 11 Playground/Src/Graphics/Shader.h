#pragma once
#include "../ErrorLogger.h"
#pragma comment (lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl\client.h>
#include <d3dcompiler.h>
class VertexShader
{
public:
	bool init(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	ID3D11VertexShader* getShader();
	ID3D10Blob* getBuffer();
	ID3D11InputLayout* getInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = nullptr;
};

class PixelShader
{
public:
	bool init(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderPath);
	ID3D11PixelShader* getShader();
	ID3D10Blob* getBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};

