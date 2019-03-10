#include "Shader.h"

bool VertexShader::init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderPath;
		ErrorLogger::log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader: ";
		errorMsg += shaderPath;
		ErrorLogger::log(hr, errorMsg);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::log(hr, "Failed to create input layout.");
		return false;
	}

	return true;
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

bool PixelShader::init(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader:";
		errorMsg += shaderPath;
		ErrorLogger::log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(shaderBuffer.Get()->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader:";
		errorMsg += shaderPath;
		ErrorLogger::log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::getShader()
{
	return shader.Get();
}

ID3D10Blob* PixelShader::getBuffer()
{
	return shaderBuffer.Get();
}
