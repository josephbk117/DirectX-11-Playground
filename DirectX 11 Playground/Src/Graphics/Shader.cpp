#include "Shader.h"

bool VertexShader::init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader :";
		errorMsg += shaderPath;
		ErrorLogger::log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());

	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader :";
		errorMsg += shaderPath;
		ErrorLogger::log(hr, errorMsg);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create input layout");
		return false;
	}

	return false;
}

ID3D11VertexShader * VertexShader::getShader()
{
	return shader.Get();
}

ID3D10Blob * VertexShader::getBuffer()
{
	return shaderBuffer.Get();
}

ID3D11InputLayout * VertexShader::getInputLayout()
{
	return inputLayout.Get();
}
